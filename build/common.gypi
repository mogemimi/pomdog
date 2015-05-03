{
  'variables': {
    'component%': 'shared_library', # static_library or shared_library
  },
  'target_defaults': {
    'msvs_configuration_attributes': {
      #'CharacterSet': '1', # Unicode
      #'OutputDirectory': '$(SolutionDir)$(ConfigurationName)',
      'IntermediateDirectory': '$(SolutionDir)..\\out\\obj\\$(ConfigurationName)\\$(ProjectName)',
    },
    'msbuild_settings': {
      'ClCompile': {
        'WarningLevel': 'Level4', # /W4
        'PreprocessorDefinitions': [
          '_WIN32_WINNT=0x0602', # Windows 8 or later
          'WIN32_LEAN_AND_MEAN',
          'NOMINMAX',
        ],
      },
    },
    'default_configuration': 'Release',
    'configurations': {
      'Debug': {
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
          'ONLY_ACTIVE_ARCH': 'YES',
        },
      }, # Debug
      'Release': {
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
