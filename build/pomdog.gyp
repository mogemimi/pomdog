{
  'includes': ['common.gypi'],
  'make_global_settings': [
    ['CXX','/usr/bin/clang++'],
    ['LINK','/usr/bin/clang++'],
  ],
  'target_defaults': {
    'dependencies': [
      #'glew.gyp:glew',
    ],
    'include_dirs': [
      '../include',
      '../third_party/boost/include',
    ],
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
    },
  },
  'targets': [
    {
      'target_name': 'pomdog-core',
      'product_name': 'PomdogCore',
      'type': 'static_library',
      'include_dirs': [
        '../include',
      ],
      'sources': [
        '../include/pomdog/Config/FundamentalTypes.hpp',
        '../include/pomdog/Config/Platform.hpp',
        '../include/pomdog/Event/Event.hpp',
        '../include/pomdog/Event/EventCode.hpp',
        '../include/pomdog/Event/EventCodeHelper.hpp',
        '../include/pomdog/Event/EventConnection.hpp',
        '../include/pomdog/Event/EventHandler.hpp',
        '../include/pomdog/Event/EventQueue.hpp',
        '../include/pomdog/Event/ScopedConnection.hpp',
        '../include/pomdog/Event/detail/EventArguments.hpp',
        '../include/pomdog/GamePlay/GameObject.cpp',
        '../include/pomdog/GamePlay/GameObject.hpp',
        '../include/pomdog/GamePlay/detail/GameComponent.hpp',
        '../include/pomdog/Math/Color.hpp',
        '../include/pomdog/Utility/Assert.hpp',
        '../include/pomdog/Utility/Exception.hpp',
        '../include/pomdog/Utility/Noncopyable.hpp',
        '../include/pomdog/Utility/detail/Tagged.hpp',
        '../src/Event/Event.cpp',
        '../src/Event/EventCodeHelper.cpp',
        '../src/Event/EventConnection.cpp',
        '../src/Event/EventHandler.cpp',
        '../src/Event/EventQueue.cpp',
        '../src/Event/detail/EventSlot.cpp',
        '../src/Event/detail/EventSlot.hpp',
        '../src/Event/detail/EventSlotCollection.cpp',
        '../src/Event/detail/EventSlotCollection.hpp',
        '../src/Math/Color.cpp',
        '../src/Utility/CRC32.cpp',
        '../src/Utility/CRC32.hpp',
        '../src/Utility/HashingHelper.hpp',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': ['-std=c++11','-stdlib=libc++'],
      },
    },
  ],# "targets"
}
