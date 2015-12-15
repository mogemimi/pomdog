# zlib.gyp - for zlib 1.2.8
{
  'targets': [
    {
      'target_name': 'zlib_static',
      'product_name': 'zlib',
      'type': 'static_library',
      'include_dirs': [
        '../../dependencies/zlib',
      ],
      'sources': [
        '../../dependencies/zlib/adler32.c',
        '../../dependencies/zlib/compress.c',
        '../../dependencies/zlib/crc32.c',
        '../../dependencies/zlib/crc32.h',
        '../../dependencies/zlib/deflate.c',
        '../../dependencies/zlib/deflate.h',
        '../../dependencies/zlib/gzclose.c',
        '../../dependencies/zlib/gzguts.h',
        '../../dependencies/zlib/gzlib.c',
        '../../dependencies/zlib/gzread.c',
        '../../dependencies/zlib/gzwrite.c',
        '../../dependencies/zlib/infback.c',
        '../../dependencies/zlib/inffast.c',
        '../../dependencies/zlib/inffast.h',
        '../../dependencies/zlib/inffixed.h',
        '../../dependencies/zlib/inflate.c',
        '../../dependencies/zlib/inflate.h',
        '../../dependencies/zlib/inftrees.c',
        '../../dependencies/zlib/inftrees.h',
        '../../dependencies/zlib/trees.c',
        '../../dependencies/zlib/trees.h',
        '../../dependencies/zlib/uncompr.c',
        '../../dependencies/zlib/zconf.h',
        '../../dependencies/zlib/zlib.h',
        '../../dependencies/zlib/zutil.c',
        '../../dependencies/zlib/zutil.h',
      ],
      'default_configuration': 'Release',
      'configurations': {
        'Debug': {
          'defines': ['_DEBUG'],
          'msbuild_settings':{
            'ClCompile': {
              'RuntimeLibrary': 'MultiThreadedDebug', # /MTd
            },
          },
        },
        'Release': {
          'defines': ['NDEBUG'],
          'msbuild_settings':{
            'ClCompile': {
              'PreprocessorDefinitions': [
                'ASMV',
                'ASMINF',
              ],
              'RuntimeLibrary': 'MultiThreaded', # /MT
            },
          },
        },
      },
      'msbuild_configuration_attributes': {
        'CharacterSet': 'Unicode',
      },
      'msbuild_settings':{
        'ClCompile': {
          'PreprocessorDefinitions': [
            'WIN32',
            '_WINDOWS',
            '_CRT_NONSTDC_NO_DEPRECATE',
            '_CRT_SECURE_NO_DEPRECATE',
            '_CRT_NONSTDC_NO_WARNINGS',
          ],
          'SuppressStartupBanner': 'true',                 # /nologo
          'WarningLevel': 'Level3',                        # /W3
          'Optimization': 'MaxSpeed',                      # /O2
          'InlineFunctionExpansion': 'OnlyExplicitInline', # /Ob1
          'StringPooling': 'true',                         # /GF
          'FunctionLevelLinking': 'true',                  # /Gy
          'BufferSecurityCheck': 'false',                  # /GS-
        },
      },
      'xcode_settings': {
        'SKIP_INSTALL': 'YES',
        'COMBINE_HIDPI_IMAGES': 'YES',
        'GCC_C_LANGUAGE_STANDARD': 'c89',
        'MACOSX_DEPLOYMENT_TARGET': '10.9',
      },
    },
  ],
}
