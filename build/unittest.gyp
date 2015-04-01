# Building under Mac OSX and Xcode:
# gyp build/unittest.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
#
# Building under Windows and MSVC2015+:
# gyp build/unittest.gyp --depth=. -f msvs -G msvs_version=2013 --generator-output=./build.msvs/
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
        'pomdog.gyp:pomdog-testable',
      ],
      'include_dirs': [
        '../include',
        '../third-party/iutest/include',
      ],
      'sources': [
        '../test/FrameworkTest/main.cpp',
        '../test/FrameworkTest/Event/ConnectionTest.cpp',
        '../test/FrameworkTest/Event/EventQueueTest.cpp',
        '../test/FrameworkTest/Event/EventTest.cpp',
        '../test/FrameworkTest/Event/ScopedConnectionTest.cpp',
        '../test/FrameworkTest/Event/SignalTest.cpp',
        #'../test/FrameworkTest/Gameplay/GameObjectIDTest.cpp',
        #'../test/FrameworkTest/Gameplay/GameObjectTest.cpp',
        #'../test/FrameworkTest/Gameplay/GameWorldTest.cpp',
        '../test/FrameworkTest/Graphics/CustomVertexTest.cpp',
        '../test/FrameworkTest/Graphics/VertexDeclarationTest.cpp',
        '../test/FrameworkTest/Input/ButtonStateTest.cpp',
        '../test/FrameworkTest/Input/KeyboardStateTest.cpp',
        '../test/FrameworkTest/Input/KeyStateTest.cpp',
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
        '../test/FrameworkTest/Utility/StringFormatTest.cpp',
        '../test/FrameworkTest/Utility/AnyTest.cpp',
      ],
      'xcode_settings': {
      },
    },
  ],# "targets"
}
