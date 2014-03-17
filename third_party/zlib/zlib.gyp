# zlib.gyp - for zlib 1.2.8
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
      'msvs_configuration_attributes': {
        #'CharacterSet': '2', # Multibyte Character Set
        #'OutputDirectory': '$(SolutionDir)$(ConfigurationName)',
      },
      'msvs_settings':{
        'VCCLCompilerTool': {
          'SuppressStartupBanner': 'true', # /nologo
          'WarningLevel': '3',             # /W3
          'Optimization': '2',             # /O2
          'InlineFunctionExpansion': '1',  # /Ob1
          'PreprocessorDefinitions': [
            'WIN32',
            'NDEBUG',
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
