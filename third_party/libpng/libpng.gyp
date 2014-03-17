# libpng.gyp - for libpng 1.6.9
{
  'target_defaults': {
    'default_configuration': 'Release',
    'configurations': {
      'Debug': {},
      'Release': {},
    },
  },
  'targets': [
    {
      'target_name': 'libpng_static',
      'product_name': 'libpng',
      'type': 'static_library',
      'dependencies': [
        '../zlib/zlib.gyp:zlib_static',
      ],
      'include_dirs': [
        '.',
      ],
      'sources': [
        'png.c',
        'png.h',
        'pngconf.h',
        'pngdebug.h',
        'pngerror.c',
        'pngget.c',
        'pnginfo.h',
        'pnglibconf.h',
        'pngmem.c',
        'pngpread.c',
        'pngread.c',
        'pngrio.c',
        'pngrtran.c',
        'pngrutil.c',
        'pngset.c',
        'pngstruct.h',
        'pngtrans.c',
        'pngwio.c',
        'pngwrite.c',
        'pngwtran.c',
        'pngwutil.c',
      ],
      'msvs_configuration_attributes': {
        #'CharacterSet': '2', # Multibyte Character Set
        #'OutputDirectory': '$(SolutionDir)$(ConfigurationName)',
      },
      'msvs_settings':{
        'VCCLCompilerTool': {
          'SuppressStartupBanner': 'true', # /nologo
          'WarningLevel': '4',             # /W4
          'Optimization': '2',             # /O2
          'InlineFunctionExpansion': '1',  # /Ob1
          'PreprocessorDefinitions': [
            'WIN32',
            'NDEBUG',
            '_WINDOWS',
          ],
          'StringPooling': 'true',              # /GF
          'RuntimeLibrary': '0',                # /MT
          'EnableFunctionLevelLinking': 'true', # /Gy
        },
      },
      'xcode_settings': {
	    'SKIP_INSTALL': 'YES',
      },
    },
  ],# "targets"
}
