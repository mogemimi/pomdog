{
  'variables': {
    'pomdog_dir%': '../..',
  },
  'includes': ['../../build/common.gypi'],
  'target_defaults': {
    'include_dirs': [
      '<@(pomdog_dir)/include',
    ],
    'configurations': {
      'Debug': {
        'msbuild_settings': {
          'Link': {
            'GenerateDebugInformation': 'true', # /DEBUG
          },
        },
      },
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
      'MACOSX_DEPLOYMENT_TARGET': '10.9',
      'CLANG_CXX_LIBRARY': 'libc++',
    },
  },
  'targets': [
    {
      'target_name': 'unittest',
      'product_name': 'unittest',
      'type': 'executable',
      'dependencies': [
        '<@(pomdog_dir)/build/pomdog.gyp:pomdog-static',
      ],
      'include_dirs': [
        '<@(pomdog_dir)/include',
        '<@(pomdog_dir)/third-party/iutest/include',
      ],
      'sources': [
        'main.cpp',
        'Application/GameClockTest.cpp',
        'Application/TimerTest.cpp',
        'Content/PathHelperTest.cpp',
        # 'Async/SchedulerTest.cpp',
        # 'Async/TaskTest.cpp',
        #'Gameplay/GameObjectIDTest.cpp',
        #'Gameplay/GameObjectTest.cpp',
        #'Gameplay/GameWorldTest.cpp',
        'Graphics/InputLayoutHelperTest.cpp',
        'Input/KeyboardStateTest.cpp',
        'Input/KeysTest.cpp',
        'Input/MouseStateTest.cpp',
        'Logging/LogChannelTest.cpp',
        'Logging/LogTest.cpp',
        'Math/BoundingBoxTest.cpp',
        'Math/BoundingBox2DTest.cpp',
        'Math/BoundingCircleTest.cpp',
        'Math/BoundingSphereTest.cpp',
        'Math/ColorTest.cpp',
        'Math/MathHelperTest.cpp',
        'Math/Matrix2x2Test.cpp',
        'Math/Matrix3x2Test.cpp',
        'Math/Matrix3x3Test.cpp',
        'Math/Matrix4x4Test.cpp',
        'Math/Point2DTest.cpp',
        'Math/Point3DTest.cpp',
        'Math/QuaternionTest.cpp',
        'Math/RayTest.cpp',
        'Math/RectangleTest.cpp',
        'Math/Vector2Test.cpp',
        'Math/Vector3Test.cpp',
        'Math/Vector4Test.cpp',
        'Signals/ConnectionTest.cpp',
        'Signals/ConnectionListTest.cpp',
        'Signals/EventQueueTest.cpp',
        'Signals/EventTest.cpp',
        'Signals/HelpersTest.cpp',
        'Signals/ScopedConnectionTest.cpp',
        'Signals/SignalTest.cpp',
        'Utility/AnyTest.cpp',
        'Utility/CRC32Test.cpp',
        'Utility/OptionalTest.cpp',
        'Utility/StringHelperTest.cpp',
      ],
    },
  ],
}
