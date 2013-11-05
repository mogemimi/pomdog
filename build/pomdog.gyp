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
        '../include/pomdog/Math/Color.hpp',
        '../include/pomdog/Utility/Assert.hpp',
        '../include/pomdog/Utility/Noncopyable.hpp',
        '../include/pomdog/Utility/detail/Tagged.hpp',
        '../src/Math/Color.cpp',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': ['-std=c++11','-stdlib=libc++'],
      },
    },
  ],# "targets"
}
