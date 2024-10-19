project('quickerRAWGL','c','cpp',
  version: '1.0.0',
  license: 'GPL-3.0-only',
  default_options : ['cpp_std=c++17', 'default_library=shared', 'buildtype=release'],
  subproject_dir : 'extern'
)

# Loading dependencies
subdir('source')

# Do not build any targets if this is a subproject
if meson.is_subproject() == false

# Common application flags
commonCompileArgs = [ '-Wall', '-Wfatal-errors' ]

# Grabbing jaffarCommon dependency

jaffarCommonSubproject = subproject('jaffarCommon')
jaffarCommonDependency = jaffarCommonSubproject.get_variable('jaffarCommonDependency')

# Building playback tool

if get_option('buildPlayer') == true

  quickerRAWGLPlayer = executable('quickerRAWGLPlayer',
    'source/player.cpp',
    cpp_args            : [ commonCompileArgs, '-DNCURSES', '-D_JAFFAR_PLAYER' ],
    dependencies        : [ quickerRAWGLDependency,
                            jaffarCommonDependency
                          ],
    include_directories : include_directories(['source']),
    link_args           : [ '-lncurses' ],
  )

endif

# Building tester tool for QuickerRAWGLGL

quickerRAWGLTester = executable('quickerRAWGLTester',
  'source/tester.cpp',
  cpp_args            : [ commonCompileArgs ], 
  dependencies        : [ quickerRAWGLDependency, jaffarCommonDependency ],
)

# Building tests
subdir('tests')

endif # If not subproject
