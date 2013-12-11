{
  'variables': {
    'conditions': [
      ['OS == "mac"', {
        'target_arch%': 'x64',
      }, {
        'target_arch%': 'ia32',
      }],
    ],
    #'target_arch%': '<(target_arch)',
    #'component%': 'shared_library', # static_library or shared_library
  },
  'target_defaults': {
    'defines': [],
    'conditions': [
      ['target_arch == "arm"', {
        # arm
      }], # target_archs == "arm"
      ['target_arch == "ia32"', {
        'xcode_settings': {
          'ARCHS': ['i386'],
        },
      }], # target_archs == "ia32"
      ['target_arch == "mipsel"', {
        # mipsel
      }], # target_archs == "mipsel"
      ['target_arch == "x64"', {
        'xcode_settings': {
          'ARCHS': ['x86_64'], # For the non-fragile objective-c ABI.
        },
      }], # target_archs == "x64"
    ],
    'msvs_configuration_attributes': {
      #'CharacterSet': '1', # Unicode
      #'OutputDirectory': '$(SolutionDir)$(ConfigurationName)',
      'IntermediateDirectory': '$(SolutionDir)..\\out\\obj\\$(ConfigurationName)\\$(ProjectName)',
    },
    'msvs_settings': {
      'VCCLCompilerTool': {
        'WarningLevel': '4',   # /W4
        'PreprocessorDefinitions': [
          '_WIN32_WINNT=0x0600', # Windows Vista or later
          'WIN32_LEAN_AND_MEAN',
          'NOMINMAX',
        ],
      },
    },
    'default_configuration': 'Release',#'Debug',
    'configurations': {
      'Common': {
        'abstract': 1,
      }, # Common
      'Debug': {
        'inherit_from': ['Common'],
        'defines': ['DEBUG=1'],
        'cflags': ['-g', '-O0'],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'Optimization': '0', # /Od
            'conditions': [
              ['OS == "win" and component == "shared_library"', {
                'RuntimeLibrary': '3', # /MDd
              }, {
                'RuntimeLibrary': '1', # /MTd
              }],
            ],
          },
          'VCLinkerTool': {
            'LinkIncremental': '2',
          },
        },
        'xcode_settings': {
          'OTHER_CFLAGS': ['-g'],
          'GCC_OPTIMIZATION_LEVEL': '0', # -O0
        },
      }, # Debug
      'Release': {
        'inherit_from': ['Common'],
        'cflags': ['-O3'],
        'msvs_settings':{
          'VCCLCompilerTool': {
            'Optimization': '2', # /O2
            'InlineFunctionExpansion': '2',
            'conditions': [
              ['OS == "win" and component == "shared_library"', {
                'RuntimeLibrary': '2', # /MD
              }, {
                'RuntimeLibrary': '0', # /MT
              }],
            ],
          },
          'VCLinkerTool': {
            'LinkIncremental': '1',
            'OptimizeReferences': '2',
          },
        },
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '3', # -O3
        },
        'conditions': [
          ['OS == "linux" or OS == "android"', {
            # Linux and Android settings
            # todo: not implemented
          }], # OS == "linux" or OS == "android"
        ], # conditions
      }, # Release
    },
  }, # target_defaults
}
