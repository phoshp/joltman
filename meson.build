project(
  'joltman',
  'cpp', 'c', 'objc',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++20', 'default_library=static'],
)

raylib_dep = subproject('raylib').get_variable('raylib_dep')

dependencies = [
  raylib_dep
]

if host_machine.system() == 'darwin'
  dependencies += dependency('appleframeworks', modules : [
    'Cocoa', 
    'IOKit', 
    'CoreVideo', 
    'CoreAudio',
    'CoreFoundation',
    'OpenGL'
  ])
elif host_machine.system() == 'windows'
  add_project_link_arguments('-static', language: ['c', 'cpp'])
endif

exe = executable(
  'joltman',
  ['src/main.cc', 'src/game.cc'],
  dependencies : dependencies,
  install : true,
)

test('basic', exe)
