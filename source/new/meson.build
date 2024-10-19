# quickerRAWGL Core sources

quickerRAWGLSrc =  [
'core/aifcplayer.cpp',
'core/bitmap.cpp',
'core/engine.cpp',
'core/file.cpp',
'core/graphics_soft.cpp',
'core/main.cpp',
'core/pak.cpp',
'core/resource_3do.cpp',
'core/resource.cpp',
'core/resource_nth.cpp',
'core/resource_win31.cpp',
'core/scaler.cpp',
'core/screenshot.cpp',
'core/script.cpp',
'core/sfxplayer.cpp',
'core/staticres.cpp',
'core/systemstub_sdl.cpp',
'core/systemstub_dummy.cpp',
'core/unpack.cpp',
'core/util.cpp',
'core/video.cpp',
]

quickerRAWGLIncludeDirs = [
 '.',
 '..',
 'core'
]

quickerRAWGLCompileArgs = [
  '-DBYPASS_PROTECTION'
]

# quickerRAWGL Core Configuration

 quickerRAWGLDependency = declare_dependency(
  compile_args        : [  quickerRAWGLCompileArgs ],
  link_args           : [  '-lz' ],
  include_directories : include_directories(quickerRAWGLIncludeDirs),
  sources             : [ quickerRAWGLSrc ],
  dependencies        : [  
                          dependency('sdl2',  required : true)
                        ]
 )