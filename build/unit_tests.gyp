# Examples:
# gyp build/unit_tests.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
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
        'WarningLevel': '4',   # /W4
        'WarnAsError': 'true', # /WX
      },
    },
    'xcode_settings': {
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
      'MACOSX_DEPLOYMENT_TARGET': '10.8', # OS X Deployment Target: 10.8
      'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
    },
  },
  'targets': [
    {
      'target_name': 'unit_tests',
      'product_name': 'unit_tests',
      'type': 'executable',
      'mac_bundle': 1,
      'dependencies': [
        'pomdog.gyp:pomdog-core',
      ],
      'include_dirs': [
        '../include',
        '../third_party/iutest/include'
      ],
      'sources': [
        '../test/FrameworkTest/main.cpp',
        '../test/FrameworkTest/Event/EventConnectionTest.cpp',
        '../test/FrameworkTest/Event/ScopedConnectionTest.cpp',
        '../test/FrameworkTest/Logging/LogChannelTest.cpp',
        '../test/FrameworkTest/Logging/LoggingTest.cpp',
        '../test/FrameworkTest/Logging/LogStreamTest.cpp',
        '../test/FrameworkTest/Math/ColorTest.cpp',
        '../test/FrameworkTest/Math/Point2DTest.cpp',
        '../test/FrameworkTest/Math/Point3DTest.cpp',
        '../test/FrameworkTest/Math/QuaternionTest.cpp',
        '../test/FrameworkTest/Math/Vector2Test.cpp',
        '../test/FrameworkTest/Math/Vector3Test.cpp',
        '../test/FrameworkTest/Math/Vector4Test.cpp',
      ],
      'xcode_settings': {
      },
      'conditions': [
        ['OS == "mac"', {
          'defines': ['IUTEST_OS_LINUX'] # badcode
        }], # OS == "mac"
      ],
    },
  ],# "targets"
}
