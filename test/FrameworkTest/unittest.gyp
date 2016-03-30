{
  'variables': {
    'pomdog_dir%': '../..',
  },
  'includes': ['../../build/common.gypi'],
  'target_defaults': {
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
      'MACOSX_DEPLOYMENT_TARGET': '10.11',
      'CLANG_CXX_LIBRARY': 'libc++',
      # Warnings (Clang):
      'CLANG_WARN_BOOL_CONVERSION': 'YES',
      'CLANG_WARN_CONSTANT_CONVERSION': 'YES',
      'CLANG_WARN_EMPTY_BODY': 'YES',
      'CLANG_WARN_ENUM_CONVERSION': 'YES',
      'CLANG_WARN_INT_CONVERSION': 'YES',
      'CLANG_WARN_UNREACHABLE_CODE': 'YES',
      # Warnings (GCC and Clang):
      'GCC_WARN_64_TO_32_BIT_CONVERSION': 'YES',
      'GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS': 'YES',
      'GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS': 'YES',
      'GCC_WARN_ABOUT_MISSING_NEWLINE': 'YES',
      'GCC_WARN_ABOUT_RETURN_TYPE': 'YES_ERROR',
      'GCC_WARN_CHECK_SWITCH_STATEMENTS': 'YES',
      'GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS': 'YES',
      'GCC_WARN_MISSING_PARENTHESES': 'YES',
      'GCC_WARN_NON_VIRTUAL_DESTRUCTOR': 'YES',
      'GCC_WARN_SHADOW': 'YES',
      'GCC_WARN_SIGN_COMPARE': 'YES',
      'GCC_WARN_TYPECHECK_CALLS_TO_PRINTF': 'YES',
      'GCC_WARN_UNINITIALIZED_AUTOS': 'YES_AGGRESSIVE',
      'GCC_WARN_UNKNOWN_PRAGMAS': 'YES',
      'GCC_WARN_UNUSED_FUNCTION': 'YES',
      'GCC_WARN_UNUSED_LABEL': 'YES',
      'GCC_WARN_UNUSED_VALUE': 'YES',
      'GCC_WARN_UNUSED_VARIABLE': 'YES',
      # Warnings - Objective-C:
      'CLANG_WARN_DIRECT_OBJC_ISA_USAGE': 'YES_ERROR',
      'CLANG_WARN__DUPLICATE_METHOD_MATCH': 'YES',
      'GCC_WARN_ALLOW_INCOMPLETE_PROTOCOL': 'YES',
      'GCC_WARN_UNDECLARED_SELECTOR': 'YES',
      'CLANG_WARN_OBJC_ROOT_CLASS': 'YES_ERROR',
      # Symbols:
      'CLANG_ENABLE_OBJC_ARC': 'YES',
      'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES', # '-fvisibility-inlines-hidden'
      'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES', # '-fvisibility=hidden'
    },
  },
  'targets': [
    {
      'target_name': 'unittest',
      'product_name': 'unittest',
      'type': 'executable',
      'dependencies': [
        #'<@(pomdog_dir)/build/pomdog.gyp:pomdog-static',
        '<@(pomdog_dir)/build/experimental.gyp:pomdog_experimental',
      ],
      'include_dirs': [
        '<@(pomdog_dir)/include',
        '<@(pomdog_dir)/experimental',
        '<@(pomdog_dir)/dependencies/iutest/include',
      ],
      'sources': [
        'main.cpp',
        'Application/GameClockTest.cpp',
        'Application/TimerTest.cpp',
        'Content/PathHelperTest.cpp',
        # 'Async/SchedulerTest.cpp',
        # 'Async/TaskTest.cpp',
        'Experimental/Gameplay/EntityIDTest.cpp',
        'Experimental/Gameplay/EntityTest.cpp',
        'Experimental/Gameplay/EntityManagerTest.cpp',
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
