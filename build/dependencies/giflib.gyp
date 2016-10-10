# giflib.gyp - for giflib 5.1.4
{
  'targets': [
    {
      'target_name': 'giflib_static',
      'product_name': 'giflib',
      'type': 'static_library',
      'include_dirs': [
        '../../dependencies/vendor/giflib/lib',
      ],
      'conditions': [
        ['OS == "win"', {
          'include_dirs': [
            '../../dependencies/vendor/giflib/windows',
          ],
        }],
      ],
      'sources': [
        '../../dependencies/vendor/giflib/lib/dgif_lib.c',
        '../../dependencies/vendor/giflib/lib/egif_lib.c',
        '../../dependencies/vendor/giflib/lib/gif_err.c',
        '../../dependencies/vendor/giflib/lib/gif_font.c',
        '../../dependencies/vendor/giflib/lib/gif_hash.c',
        '../../dependencies/vendor/giflib/lib/gif_hash.h',
        '../../dependencies/vendor/giflib/lib/gif_lib_private.h',
        '../../dependencies/vendor/giflib/lib/gif_lib.h',
        '../../dependencies/vendor/giflib/lib/gifalloc.c',
        '../../dependencies/vendor/giflib/lib/openbsd-reallocarray.c',
        '../../dependencies/vendor/giflib/lib/quantize.c',
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
