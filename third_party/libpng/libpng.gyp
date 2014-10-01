# libpng.gyp - for libpng 1.6.12
{
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
        '../zlib',
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
      'default_configuration': 'Release',
      'configurations': {
        'Debug': {
          'msvs_settings':{
            'VCCLCompilerTool': {
              'RuntimeLibrary': '1', # /MTd
            },
          },
        },
        'Release': {
          'defines': ['NDEBUG'],
            'msvs_settings':{
              'VCCLCompilerTool': {
                'RuntimeLibrary': '0', # /MT
              },
            },
        },
      },
      'msvs_configuration_attributes': {
        'CharacterSet': '2', # Multibyte Character Set
      },
      'msvs_settings':{
        'VCCLCompilerTool': {
          'SuppressStartupBanner': 'true',    # /nologo
          'WarningLevel': '4',                # /W4
          'Optimization': '3',                # /Ox
          'EnableIntrinsicFunctions': 'true', # /Oi
          'WholeProgramOptimization': 'true', # /GL
          'PreprocessorDefinitions': [
            'WIN32',
            '_WINDOWS',
          ],
          'MinimalRebuild': 'false',            # /Gm-
          'EnableFunctionLevelLinking': 'true', # /Gy
          'FloatingPointExceptions': 'false',   # /fp:except-
          'CompileAs': '1',                     # /TC
          'StringPooling': 'true',              # /GF
          'DisableSpecificWarnings': ['4996'],  # /wd"4996"
        },
        'VCLibrarianTool': {
          'LinkTimeCodeGeneration': 'true', # /LTCG
        },
      },
      'xcode_settings': {
        'SKIP_INSTALL': 'YES',
      },
    },
  ],
}
