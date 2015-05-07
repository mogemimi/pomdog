{
  'includes': ['common.gypi'],
  'make_global_settings': [
    ['CXX','/usr/bin/clang++'],
    ['LINK','/usr/bin/clang++'],
  ],
  'target_defaults': {
    'include_dirs': [
      '../include',
    ],
    'configurations': {
      'Debug': {
        'msbuild_settings': {
          'Link': {
            'GenerateDebugInformation': 'true', # /DEBUG
          },
        },
      }, # Debug
    },
    'msbuild_settings': {
      'ClCompile': {
        'WarningLevel': 'Level4', # /W4
        'TreatWarningAsError': 'true', # /WX
      },
    },
    'xcode_settings': {
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++14',
      'MACOSX_DEPLOYMENT_TARGET': '10.8', # OS X Deployment Target: 10.8
      'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
    },
  },
  'targets': [
    {
      'target_name': 'unittest',
      'product_name': 'unittest',
      'type': 'executable',
      'dependencies': [
        'pomdog.gyp:pomdog-static',
      ],
      'include_dirs': [
        '../include',
        '../third-party/iutest/include',
      ],
      'sources': [
        '../test/FrameworkTest/main.cpp',
        #'../test/FrameworkTest/Gameplay/GameObjectIDTest.cpp',
        #'../test/FrameworkTest/Gameplay/GameObjectTest.cpp',
        #'../test/FrameworkTest/Gameplay/GameWorldTest.cpp',
        '../test/FrameworkTest/Graphics/InputLayoutHelperTest.cpp',
        '../test/FrameworkTest/Input/KeyboardStateTest.cpp',
        '../test/FrameworkTest/Input/KeysTest.cpp',
        '../test/FrameworkTest/Input/MouseStateTest.cpp',
        '../test/FrameworkTest/Logging/LogChannelTest.cpp',
        '../test/FrameworkTest/Logging/LogStreamTest.cpp',
        '../test/FrameworkTest/Logging/LogTest.cpp',
        '../test/FrameworkTest/Math/ColorTest.cpp',
        '../test/FrameworkTest/Math/MathHelperTest.cpp',
        '../test/FrameworkTest/Math/Matrix2x2Test.cpp',
        '../test/FrameworkTest/Math/Matrix3x2Test.cpp',
        '../test/FrameworkTest/Math/Matrix3x3Test.cpp',
        '../test/FrameworkTest/Math/Matrix4x4Test.cpp',
        '../test/FrameworkTest/Math/Point2DTest.cpp',
        '../test/FrameworkTest/Math/Point3DTest.cpp',
        '../test/FrameworkTest/Math/QuaternionTest.cpp',
        '../test/FrameworkTest/Math/Vector2Test.cpp',
        '../test/FrameworkTest/Math/Vector3Test.cpp',
        '../test/FrameworkTest/Math/Vector4Test.cpp',
        '../test/FrameworkTest/Signals/ConnectionTest.cpp',
        '../test/FrameworkTest/Signals/EventQueueTest.cpp',
        '../test/FrameworkTest/Signals/EventTest.cpp',
        '../test/FrameworkTest/Signals/ScopedConnectionTest.cpp',
        '../test/FrameworkTest/Signals/SignalTest.cpp',
        '../test/FrameworkTest/Utility/AnyTest.cpp',
        '../test/FrameworkTest/Utility/OptionalTest.cpp',
        '../test/FrameworkTest/Utility/StringFormatTest.cpp',
      ],
      'xcode_settings': {
      },
    },
  ],# "targets"
}
