# Examples:
# gyp build/trivial_test.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
# gyp build/trivial_test.gyp --depth=. -f xcode -D target_arch=ia32
# gyp build/trivial_test.gyp --depth=. -f xcode -D target_arch=x64
{
  'includes': ['common.gypi'],
  'make_global_settings': [
    ['CXX','/usr/bin/clang++'],
    ['LINK','/usr/bin/clang++'],
  ],
  'target_defaults': {
    'include_dirs': [
      '../include',
      '../third_party/boost/include',
    ],
    'configurations': {
      'Debug': {
        'msvs_settings': {
          'VCLinkerTool': {
            'GenerateDebugInformation': 'true', # /DEBUG
          },
        },
      }, # Debug
    },
    'msvs_settings':{
      'VCCLCompilerTool': {
        'WarningLevel': '4',    # /W4
        #'WarnAsError': 'true', # /WX
      },
    },
    'xcode_settings': {
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
      'MACOSX_DEPLOYMENT_TARGET': '10.8', # OS X Deployment Target: 10.8
    },
  },
  'targets': [
    {
      'target_name': 'trivial_test',
      'product_name': 'TrivialTest',
      'type': 'executable',
      'mac_bundle': 1,
      'dependencies': [
        'pomdog.gyp:pomdog-core',
      ],
      'include_dirs': [
        '../include',
        '../test/TrivialTest'
      ],
      'sources': [
        '../test/TrivialTest/main.cpp',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': ['-std=c++11','-stdlib=libc++'],
        'OTHER_LDFLAGS': ['-stdlib=libc++'],
      },
    },
  ],# "targets"
}
