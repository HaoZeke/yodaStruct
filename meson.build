project(
  'luadseams',
  ['cpp', 'c'],
  version: '2.4.0',
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
yds_dep = seams.get_variable('yds_dep')

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

link_args = []
if host_machine.system() == 'linux'
  link_args += ['-Wl,--enable-new-dtags']
endif

dseams_core = shared_module(
  'dseams_core',
  files('src/lua_api.cpp', 'src/luaopen.cpp'),
  name_prefix: '',
  dependencies: [yds_dep, lua_dep],
  include_directories: inc,
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

lua_exe = find_program('lua', 'lua5.4', 'lua5.3', required: false)
if lua_exe.found()
  test(
    'dseams_library_read',
    lua_exe,
    args: [meson.project_source_root() / 'example_lua/library/read.lua'],
    env: {
      'LUA_PATH': meson.project_source_root() / 'lua' / '?.lua;;',
      'LUA_CPATH': meson.current_build_dir() / '?.so;;',
    },
    workdir: meson.project_source_root(),
    timeout: 120,
  )
endif
