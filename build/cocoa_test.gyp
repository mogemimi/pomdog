# Examples:
# gyp build/cocoa_tests.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
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
    },
  },
  'targets': [
    {
      'target_name': 'cocoa_test',
      'product_name': 'CocoaTest',
      'type': 'executable',
      'mac_bundle': 1,
      'dependencies': [
        'pomdog.gyp:pomdog-core',
      ],
      'include_dirs': [
        '../include',
      ],
      'sources': [
      ],
      'conditions': [
        ['OS == "win"', {
          'sources': [
          ],
        }], # OS == "win"
        ['OS == "mac"', {
          'sources': [
            '../test/CocoaTest/main.mm',
            '../test/CocoaTest/CocoaTest-Prefix.pch',
            '../test/CocoaTest/AppDelegate.h',
            '../test/CocoaTest/AppDelegate.mm',
            '../test/CocoaTest/CocoaOpenGLView.h',
            '../test/CocoaTest/CocoaOpenGLView.mm',
          ],
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
              '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
            ],
          },
        }], # OS == "mac"
      ],
      'mac_bundle_resources': [
		'../test/CocoaTest/Base.lproj/MainMenu.xib',
        '../test/CocoaTest/English.lproj/InfoPlist.strings',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': '../test/CocoaTest/CocoaTest-Info.plist',
        'OTHER_CPLUSPLUSFLAGS': ['-std=c++11','-stdlib=libc++'],
        'OTHER_LDFLAGS': ['-stdlib=libc++'],
      },
    },
  ],# "targets"
}
