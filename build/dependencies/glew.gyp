{
  'targets': [
    {
      'target_name': 'glew_static',
      'product_name': 'glew32s',
      'type': 'static_library',
      'include_dirs': [
        '../../dependencies/vendor/glew/include',
      ],
      'defines': [
        'GLEW_STATIC',
      ],
      'sources': [
        '../../dependencies/vendor/glew/include/GL/glew.h',
        '../../dependencies/vendor/glew/include/GL/glxew.h',
        '../../dependencies/vendor/glew/include/GL/wglew.h',
        '../../dependencies/vendor/glew/src/glew.c',
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
              'RuntimeLibrary': 'MultiThreaded', # /MT
            },
          },
        },
      },
      'msbuild_configuration_attributes': {
        'CharacterSet': 'Multibyte',
      },
      'msbuild_settings': {
        'ClCompile': {
          'PreprocessorDefinitions': [
            'WIN32',
            '_LIB',
            'WIN32_LEAN_AND_MEAN',
          ],
          'SuppressStartupBanner': 'true',                 # /nologo
          'WarningLevel': 'Level3',                        # /W3
          'Optimization': 'MaxSpeed',                      # /O2
          'InlineFunctionExpansion': 'OnlyExplicitInline', # /Ob1
          'StringPooling': 'true',                         # /GF
          'FunctionLevelLinking': 'true',                  # /Gy
        },
        'Lib': {
          'SuppressStartupBanner': 'true', # /NOLOGO
          'TargetMachine': 'MachineX86',
        },
      },
      'xcode_settings': {
        'MACOSX_DEPLOYMENT_TARGET': '10.9',
      },
    },
  ],
}
