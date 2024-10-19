# quickerRAW Core sources

quickerRAWSrc =  [
'core/aifcplayer.cpp',
'core/bitmap.cpp',
'core/engine.cpp',
'core/file.cpp',
'core/graphics_soft.cpp',
'core/main.cpp',
'core/mixer.cpp',
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

quickerRAWIncludeDirs = [
 '.',
 '..',
 'core'
]

quickerRAWCompileArgs = [
  '-DBYPASS_PROTECTION'
]

# quickerRAW Core Configuration

 quickerRAWDependency = declare_dependency(
  compile_args        : [  quickerRAWCompileArgs ],
  link_args           : [  '-lz' ],
  include_directories : include_directories(quickerRAWIncludeDirs),
  sources             : [ quickerRAWSrc ],
  dependencies        : [  
                          dependency('sdl2',  required : true),
                          dependency('sdl2_mixer',  required : true),
                        ]
 )