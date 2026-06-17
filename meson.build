project(
  'joltman',
  'cpp',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++20'],
)

raylib_dep = subproject('raylib').get_variable('raylib_dep')

dependencies = [
  raylib_dep
]

exe = executable(
  'joltman',
  ['src/main.cc', 'src/game.cc'],
  dependencies : dependencies,
  install : true,
)

test('basic', exe)
