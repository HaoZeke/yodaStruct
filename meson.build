project(
    'luadseams',
    ['cpp', 'c'],
    version: '2.6.0',
    default_options: ['warning_level=1', 'cpp_std=c++20'],
    meson_version: '>=1.3.0',
)

# C++ engine. This repo is the Lua/Fennel library (require("dseams")).
seams = subproject(
    'seams-core',
    default_options: [
        'default_library=static',
        'with_python=false',
        'with_tests=false',
        'with_cli=false',
        'with_lua=disabled',
    ],
)
# Eigen/BLAS inject -I/usr/include. Mark those as system includes so
# the Lua 5.3/5.4 -I wins over a distro lua.h in /usr/include.
yds_dep = seams.get_variable('yds_dep').as_system()

# Vendored sol does not support Lua 5.5 (LUA_ERRGCMM is gone).
# conda-forge gcc also drops `-I/usr/include` from the distro lua
# 5.5 pc file, so sol cannot include lua.h. Prefer 5.3/5.4 with a
# visible header; the wrap is last resort.
cpp = meson.get_compiler('cpp')
lua_dep = disabler()
foreach name : ['lua-5.4', 'lua5.4', 'lua-5.3', 'lua5.3', 'lua']
    if not lua_dep.found()
        cand = dependency(name, version: ['>=5.3', '<5.5'], required: false)
        if cand.found() and cpp.has_header('lua.h', dependencies: cand)
            lua_dep = cand
        endif
    endif
endforeach
if not lua_dep.found()
    lua_dep = dependency(
        'lua-5.4',
        version: ['>=5.3', '<5.5'],
        fallback: ['lua', 'lua_dep'],
    )
endif

inc = include_directories('src/include/internal', 'src/include/external')

# sol does `#include <lua.hpp>` when __has_include(<lua.hpp>) is
# true. That is true whenever a distro 5.5 lua.hpp sits in
# /usr/include, even if a 5.3/5.4 -I is already on the line.
# Write a build-dir lua.hpp that includes the pkg-config headers
# by absolute path so -I<builddir> wins.
fs = import('fs')
lua_includedir = lua_dep.get_variable('includedir', default_value: '')
lua_h = ''
lua_lualib_h = ''
lua_lauxlib_h = ''
# Versioned subdirs first. includedir itself is often /usr/include,
# which holds a distro 5.5 lua.h next to lua5.4/lua.h.
foreach base : [lua_includedir / 'lua5.4', lua_includedir / 'lua5.3', lua_includedir]
    cand = base / 'lua.h'
    if lua_h == '' and base != '' and fs.exists(cand)
        # 5.5 headers define LUA_VERSION_MINOR_N; 5.3/5.4 do not.
        if not fs.read(cand).contains('LUA_VERSION_MINOR_N')
            lua_h = cand
            lua_lualib_h = base / 'lualib.h'
            lua_lauxlib_h = base / 'lauxlib.h'
        endif
    endif
endforeach
if lua_h == ''
    error(
        'Lua 5.3/5.4 lua.h not found under includedir=@0@'.format(
            lua_includedir,
        ),
    )
endif
lua_conf = configuration_data()
lua_conf.set('LUA_H', lua_h)
lua_conf.set('LUALIB_H', lua_lualib_h)
lua_conf.set('LAUXLIB_H', lua_lauxlib_h)
configure_file(
    input: 'src/lua_picked.hpp.in',
    output: 'lua.hpp',
    configuration: lua_conf,
)
lua_inc_args = ['-I' + meson.current_build_dir()]
add_project_arguments(lua_inc_args, language: 'cpp')

link_args = []
if host_machine.system() == 'linux'
    link_args += ['-Wl,--enable-new-dtags']
endif

dseams_core = shared_module(
    'dseams_core',
    files('src/lua_api.cpp', 'src/luaopen.cpp'),
    name_prefix: '',
    dependencies: [lua_dep, yds_dep],
    include_directories: inc,
    cpp_args: lua_inc_args,
    link_args: link_args,
    install: true,
)

install_data(
    ['lua/dseams.lua', 'lua/dseams.fnl', 'lua/yoda.lua'],
    install_dir: get_option('datadir') / 'luadseams' / 'lua',
)
install_data(
    [
        'src/include/external/fennel/fennel.lua',
        'src/include/external/fennel/LICENSE-fennel',
    ],
    install_dir: get_option('datadir') / 'luadseams',
)

lua_exe = find_program('lua5.4', 'lua-5.4', 'lua5.3', 'lua', required: false)
if lua_exe.found()
  lua_test_env = {
    'LUA_PATH': meson.project_source_root() / 'lua' / '?.lua;;',
    'LUA_CPATH': meson.current_build_dir() / '?.so;;',
  }
  test(
    'dseams_library_read',
    lua_exe,
    args: [meson.project_source_root() / 'example_lua/library/read.lua'],
    env: lua_test_env,
    workdir: meson.project_source_root(),
    timeout: 120,
  )
  test(
    'dseams_library_rdf3d',
    lua_exe,
    args: [meson.project_source_root() / 'example_lua/library/rdf3d.lua'],
    env: lua_test_env,
    workdir: meson.project_source_root(),
    timeout: 120,
  )
  test(
    'dseams_library_runningcn',
    lua_exe,
    args: [meson.project_source_root() / 'example_lua/library/runningcn.lua'],
    env: lua_test_env,
    workdir: meson.project_source_root(),
    timeout: 120,
  )
  test(
    'dseams_library_cn',
    lua_exe,
    args: [meson.project_source_root() / 'example_lua/library/cn.lua'],
    env: lua_test_env,
    workdir: meson.project_source_root(),
    timeout: 120,
  )
  test(
    'dseams_library_workflows',
    lua_exe,
    args: [meson.project_source_root() / 'example_lua/library/workflows.lua'],
    env: lua_test_env,
    workdir: meson.project_source_root(),
    timeout: 120,
  )
  test(
    'dseams_fennel_workflows',
    lua_exe,
    args: [meson.project_source_root() / 'example_lua/library/fennel_workflows.lua'],
    env: lua_test_env,
    workdir: meson.project_source_root(),
    timeout: 120,
  )
endif
