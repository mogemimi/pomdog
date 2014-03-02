# Examples(Xcode):
# gyp build/app.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
#
# Examples(MSVS 2013):
# gyp build/app.gyp --depth=. -f msvs -G msvs_version=2013 --generator-output=./build.msvc/

{
  'includes': ['common.gypi'],
  'make_global_settings': [
    ['CXX','/usr/bin/clang++'],
    ['LINK','/usr/bin/clang++'],
  ],
  'target_defaults': {
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
      },
    },
    'xcode_settings': {
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
      'MACOSX_DEPLOYMENT_TARGET': '10.9', # OS X Deployment Target: 10.9
      'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
    },
    'include_dirs': [
      '../Pomdog/include',
    ],
    'conditions': [
      ['component == "shared_library"', {
        'dependencies': [
          '../Pomdog/build/pomdog.gyp:pomdog-shared',
        ],
        'defines': [
          'POMDOG_USING_LIBRARY_EXPORTS=1',
        ],
      }, {
        'dependencies': [
          '../Pomdog/build/pomdog.gyp:pomdog-static',
        ],
      }],
      ['OS == "win"', {
          'sources': [
          ],
      }], # OS == "win"
      ['OS == "mac"', {
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
              '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
            ],
          },
      }], # OS == "mac"
    ],
  },
  'targets': [
    {
      'target_name': 'QuickStart',
      'product_name': 'QuickStartApp',
      'type': 'executable',
      'mac_bundle': 1,
      'sources': [
        '../Source/QuickStartGame.cpp',
        '../Source/QuickStartGame.hpp',
      ],
      'conditions': [
        ['OS == "win"', {
          'sources': [
          ],
        }], # OS == "win"
        ['OS == "mac"', {
          'sources': [
            '../Platform.Cocoa/main.mm',
            '../Platform.Cocoa/QuickStart-Prefix.pch',
            '../Platform.Cocoa/AppDelegate.h',
            '../Platform.Cocoa/AppDelegate.mm',
          ],
        }], # OS == "mac"
      ],
      'mac_bundle_resources': [
        '../Platform.Cocoa/Base.lproj/MainMenu.xib',
        '../Platform.Cocoa/English.lproj/InfoPlist.strings',
        #'../Platform.Cocoa/Images.xcassets/',
        '../Content/',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': '../Platform.Cocoa/QuickStart-Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
      },
    },
  ],# "targets"
}
