project(
  'yodaStruct',
  ['cpp', 'c'],
  version: '2.0.1',
  default_options: ['warning_level=1', 'cpp_std=c++20'],
  meson_version: '>=1.3.0',
)

# C++ engine. This repo is the Lua/Fennel CLI only.
seams = subproject(
  'seams-core',
  default_options: [
    'default_library=static',
    'with_python=false',
    'with_tests=false',
    'with_lua=disabled',
  ],
)
yds_dep = seams.get_variable('yds_dep')

lua_dep = dependency('lua', required: false)
if not lua_dep.found()
  lua_dep = dependency('lua-5.4', required: false)
endif
if not lua_dep.found()
  lua_dep = dependency('lua-5.4', fallback: ['lua', 'lua_dep'])
endif

inc = include_directories('src/include/internal', 'src/include/external')

fennel_lua = meson.project_source_root() / 'src/include/external/fennel/fennel.lua'
install_data(
  [
    'src/include/external/fennel/fennel.lua',
    'src/include/external/fennel/LICENSE-fennel',
  ],
  install_dir: get_option('datadir') / 'yodaStruct',
)

link_args = []
if host_machine.system() == 'linux'
  link_args += ['-Wl,--enable-new-dtags']
endif

yoda = executable(
  'yodaStruct',
  files('src/main.cpp', 'src/lua_api.cpp', 'src/opt_parser.cpp'),
  dependencies: [yds_dep, lua_dep],
  include_directories: inc,
  cpp_args: [
    '-DYODA_FENNEL_LUA="' + fennel_lua + '"',
    '-DYODA_VERSION="' + meson.project_version() + '"',
  ],
  link_args: link_args,
  install: true,
)

test(
  'yoda_fennel',
  yoda,
  args: ['-c', 'example_lua/fennel/config.yml'],
  workdir: meson.project_source_root(),
  timeout: 120,
)
test(
  'yoda_fennel_full',
  yoda,
  args: ['-c', 'example_lua/fennel_full/config.yml'],
  workdir: meson.project_source_root(),
  timeout: 240,
)
test(
  'yoda_full_api',
  yoda,
  args: ['-c', 'example_lua/full_api/config.yml'],
  workdir: meson.project_source_root(),
  timeout: 240,
)
test(
  'yoda_structure_desc',
  yoda,
  args: ['-c', 'example_lua/structureDesc/config.yml'],
  workdir: meson.project_source_root(),
  timeout: 120,
)
