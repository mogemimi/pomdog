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
    'component%': 'shared_library', # static_library or shared_library
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
    'msbuild_settings': {
      'ClCompile': {
        'WarningLevel': 'Level4', # /W4
        'PreprocessorDefinitions': [
          '_WIN32_WINNT=0x0601', # Windows 7 or later
          'WIN32_LEAN_AND_MEAN',
          'NOMINMAX',
        ],
      },
    },
    'default_configuration': 'Release',
    #'default_configuration': 'Debug',
    'configurations': {
      'Common': {
        'abstract': 1,
      }, # Common
      'Debug': {
        'inherit_from': ['Common'],
        'defines': ['DEBUG=1'],
        'cflags': ['-g', '-O0'],
        'msbuild_settings': {
          'ClCompile': {
            'Optimization': 'Disabled', # /Od
            'conditions': [
              ['OS == "win" and component == "shared_library"', {
                'RuntimeLibrary': 'MultiThreadedDebugDLL', # /MDd
              }, {
                'RuntimeLibrary': 'MultiThreadedDebug', # /MTd
              }],
            ],
          },
          'Link': {
            #'LinkIncremental': 'true', # /INCREMENTAL
          },
        },
        'xcode_settings': {
          'OTHER_CFLAGS': ['-g'],
          'GCC_OPTIMIZATION_LEVEL': '0', # -O0
        },
      }, # Debug
      'Release': {
        'inherit_from': ['Common'],
        'defines': ['NDEBUG=1'],
        'cflags': ['-O3'],
        'msbuild_settings': {
          'ClCompile': {
            'Optimization': 'MaxSpeed', # /O2
            'InlineFunctionExpansion': 'AnySuitable', # /Ob2
            'conditions': [
              ['OS == "win" and component == "shared_library"', {
                'RuntimeLibrary': 'MultiThreadedDLL', # /MD
              }, {
                'RuntimeLibrary': 'MultiThreaded', # /MT
              }],
            ],
          },
          'Link': {
            #'LinkIncremental': 'false', # /INCREMENTAL:NO
            'OptimizeReferences': 'true', # /OPT:REF
          },
        },
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '3', # -O3
        },
      }, # Release
    },
  }, # target_defaults
}
