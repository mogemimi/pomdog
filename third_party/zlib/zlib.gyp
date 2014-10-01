# zlib.gyp - for zlib 1.2.8
{
  'targets': [
    {
      'target_name': 'zlib_static',
      'product_name': 'zlib',
      'type': 'static_library',
      'include_dirs': [
        '.',
      ],
      'sources': [
        'adler32.c',
        'compress.c',
        'crc32.c',
        'crc32.h',
        'deflate.c',
        'deflate.h',
        'gzclose.c',
        'gzguts.h',
        'gzlib.c',
        'gzread.c',
        'gzwrite.c',
        'infback.c',
        'inffast.c',
        'inffast.h',
        'inffixed.h',
        'inflate.c',
        'inflate.h',
        'inftrees.c',
        'inftrees.h',
        'trees.c',
        'trees.h',
        'uncompr.c',
        'zconf.h',
        'zlib.h',
        'zutil.c',
        'zutil.h',
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
              'PreprocessorDefinitions': [
                'ASMV',
                'ASMINF',
              ],
              'RuntimeLibrary': '0', # /MT
            },
          },
        },
      },
      'msvs_configuration_attributes': {
        'CharacterSet': '1', # Unicode Character Set
      },
      'msvs_settings':{
        'VCCLCompilerTool': {
          'PreprocessorDefinitions': [
            'WIN32',
            'ZLIB_WINAPI',
            '_CRT_NONSTDC_NO_DEPRECATE',
            '_CRT_SECURE_NO_DEPRECATE',
            '_CRT_NONSTDC_NO_WARNINGS',
          ],
          'SuppressStartupBanner': 'true',      # /nologo
          'WarningLevel': '3',                  # /W3
          'Optimization': '2',                  # /O2
          'InlineFunctionExpansion': '1',       # /Ob1
          'StringPooling': 'true',              # /GF
          'EnableFunctionLevelLinking': 'true', # /Gy
          'BufferSecurityCheck': 'false',       # /GS-
        },
      },
      'xcode_settings': {
        'SKIP_INSTALL': 'YES',
      },
    },
  ],
}
