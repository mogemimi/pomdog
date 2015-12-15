# libpng.gyp - for libpng 1.6.12
{
  'targets': [
    {
      'target_name': 'libpng_static',
      'product_name': 'libpng',
      'type': 'static_library',
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'MACOSX_DEPLOYMENT_TARGET': '10.9',
          },
        }],
        ['OS == "ios"', {
          'xcode_settings': {
            'IPHONEOS_DEPLOYMENT_TARGET': '8.0',
            'SDKROOT': 'iphoneos',
          },
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/usr/lib/libz.dylib',
            ],
          },
          'include_dirs': [
            '$(SDKROOT)/usr/include',
          ],
        }, {
          'dependencies': [
            'zlib.gyp:zlib_static',
          ],
          'include_dirs': [
            '../../dependencies/zlib',
          ],
        }],
      ],
      'include_dirs': [
        '../../dependencies/libpng',
        '../../dependencies/vendor/libpng',
      ],
      'sources': [
        '../../dependencies/vendor/libpng/pnglibconf.h',
        '../../dependencies/libpng/png.c',
        '../../dependencies/libpng/png.h',
        '../../dependencies/libpng/pngconf.h',
        '../../dependencies/libpng/pngdebug.h',
        '../../dependencies/libpng/pngerror.c',
        '../../dependencies/libpng/pngget.c',
        '../../dependencies/libpng/pnginfo.h',
        '../../dependencies/libpng/pngmem.c',
        '../../dependencies/libpng/pngpread.c',
        '../../dependencies/libpng/pngpriv.h',
        '../../dependencies/libpng/pngread.c',
        '../../dependencies/libpng/pngrio.c',
        '../../dependencies/libpng/pngrtran.c',
        '../../dependencies/libpng/pngrutil.c',
        '../../dependencies/libpng/pngset.c',
        '../../dependencies/libpng/pngstruct.h',
        '../../dependencies/libpng/pngtrans.c',
        '../../dependencies/libpng/pngwio.c',
        '../../dependencies/libpng/pngwrite.c',
        '../../dependencies/libpng/pngwtran.c',
        '../../dependencies/libpng/pngwutil.c',
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
          'msbuild_settings': {
            'ClCompile': {
              'RuntimeLibrary': 'MultiThreaded', # /MT
            },
            'Link': {
              'EnableCOMDATFolding': 'true',
              'OptimizeReferences': 'true',
            },
          },
        },
      },
      'msbuild_configuration_attributes': {
        'CharacterSet': 'MultiByte',
      },
      'msbuild_settings': {
        'ClCompile': {
          'SuppressStartupBanner': 'true',    # /nologo
          'WarningLevel': 'Level4',           # /W4
          'Optimization': 'Full',             # /Ox
          'IntrinsicFunctions': 'true',       # /Oi
          #'WholeProgramOptimization': 'true', # /GL
          'PreprocessorDefinitions': [
            'WIN32',
            '_WINDOWS',
          ],
          'MinimalRebuild': 'false',          # /Gm-
          'FunctionLevelLinking': 'true',     # /Gy
          'FloatingPointExceptions': 'false', # /fp:except-
          'CompileAs': 'CompileAsC',          # /TC
          'StringPooling': 'true',            # /GF
          'DisableSpecificWarnings': [
            '4996', '4127'
          ], # /wd"4996"
          'TreatWChar_tAsBuiltInType': 'false', # /Zc:wchar_t-
        },
        'Lib': {
          #'LinkTimeCodeGeneration': 'true', # /LTCG
        },
      },
      'xcode_settings': {
        'SKIP_INSTALL': 'YES',
        'COMBINE_HIDPI_IMAGES': 'YES',
      },
    },
  ],
}
