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
          'MACOSX_DEPLOYMENT_TARGET': '10.11',
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
            'Platform.CocoaGL4/main.mm',
            'Platform.CocoaGL4/AppDelegate.h',
            'Platform.CocoaGL4/AppDelegate.mm',
            'Platform.CocoaGL4/GameViewController.h',
            'Platform.CocoaGL4/GameViewController.mm',
          ],
          'xcode_settings': {
            'INFOPLIST_FILE': 'Platform.CocoaGL4/Info.plist',
            'LD_RUNPATH_SEARCH_PATHS': [
              '$(inherited)',
              '@executable_path/../Frameworks',
            ],
          },
          'mac_bundle_resources': [
            'Platform.CocoaGL4/Assets.xcassets/',
            'Platform.CocoaGL4/Base.lproj/MainMenu.xib',
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
