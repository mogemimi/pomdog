# Examples(Xcode):
# gyp build/cocoa_test.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
#
# Examples(MSVS 2013):
# gyp build/cocoa_test.gyp --depth=. -f msvs -G msvs_version=2013 --generator-output=./build.msvc/

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
      'target_name': 'cocoa_test',
      'product_name': 'CocoaTest',
      'type': 'executable',
      'mac_bundle': 1,
      'include_dirs': [
        '../include',
      ],
      'sources': [
      ],
      'conditions': [
        ['component == "shared_library"', {
          'dependencies': [
            'pomdog.gyp:pomdog-shared',
          ],
        }, {
          'dependencies': [
            'pomdog.gyp:pomdog-static',
          ],
        }],
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
            '../test/CocoaTest/CocoaTestGame.cpp',
            '../test/CocoaTest/CocoaTestGame.hpp',
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
        'CLANG_ENABLE_OBJC_ARC': 'YES',
      },
    },
  ],# "targets"
}
