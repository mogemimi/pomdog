{
  'includes': ['common.gypi'],
  'variables': {
    'pomdog_dir%': 'ThirdParty/pomdog',
  },
  'target_defaults': {
    'msbuild_settings': {
      'ClCompile': {
        'WarningLevel': 'Level4', # /W4
      },
    },
    'xcode_settings': {
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++14',
      'CLANG_CXX_LIBRARY': 'libc++',
      'CLANG_ENABLE_OBJC_ARC': 'YES',
    },
    'include_dirs': [
      '<@(pomdog_dir)/include',
    ],
    'conditions': [
      ['component == "shared_library"', {
        'dependencies': [
          '<@(pomdog_dir)/build/pomdog.gyp:pomdog-shared',
        ],
        'defines': [
          'POMDOG_USING_LIBRARY_EXPORTS=1',
        ],
      }, {
        'dependencies': [
          '<@(pomdog_dir)/build/pomdog.gyp:pomdog-static',
        ],
      }],
      ['OS == "mac"', {
        'xcode_settings': {
          'MACOSX_DEPLOYMENT_TARGET': '10.9',
        },
        'link_settings': {
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          ],
        },
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'QuickStart',
      'product_name': 'QuickStart',
      'type': 'executable',
      'mac_bundle': 1,
      'sources': [
        'Source/QuickStartGame.cpp',
        'Source/QuickStartGame.hpp',
      ],
      'conditions': [
        ['OS == "win"', {
          'sources': [
            'Platform.Win32/main.cpp',
            'Platform.Win32/Resource.hpp',
            'Platform.Win32/game.rc',
          ],
        }],
        ['OS == "mac"', {
          'sources': [
            'Platform.Cocoa/main.mm',
            'Platform.Cocoa/AppDelegate.h',
            'Platform.Cocoa/AppDelegate.mm',
            'Platform.Cocoa/GameView.h',
            'Platform.Cocoa/GameView.mm',
            'Platform.Cocoa/GameViewController.h',
            'Platform.Cocoa/GameViewController.mm',
          ],
          'xcode_settings': {
            'INFOPLIST_FILE': 'Platform.Cocoa/Info.plist',
            'LD_RUNPATH_SEARCH_PATHS': [
              '$(inherited)',
              '@executable_path/../Frameworks',
            ],
          },
          'mac_bundle_resources': [
            'Platform.Cocoa/Base.lproj/MainMenu.xib',
            'Platform.Cocoa/English.lproj/InfoPlist.strings',
            'Platform.Cocoa/Images.xcassets/',
          ],
        }],
        ['OS == "linux"', {
          'sources': [
            'Platform.X11/main.cpp',
          ],
        }],
      ],
      'mac_bundle_resources': [
        'Content/',
      ],
    },
  ],
}
