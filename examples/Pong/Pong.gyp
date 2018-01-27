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
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++17',
      'MACOSX_DEPLOYMENT_TARGET': '10.11',
      'CLANG_CXX_LIBRARY': 'libc++',
    },
    'include_dirs': [
      '<@(pomdog_dir)/include',
      '<@(pomdog_dir)/experimental',
    ],
    'dependencies': [
      '<@(pomdog_dir)/build/experimental.gyp:pomdog_experimental',
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
      'target_name': 'Pong',
      'product_name': 'Pong',
      'type': 'executable',
      'mac_bundle': 1,
      'sources': [
        'Source/PongGame.cpp',
        'Source/PongGame.hpp',
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
        }],
        ['OS == "linux"', {
          'sources': [
            'Platform.X11/main.cpp',
          ],
        }],
      ],
      'mac_bundle_resources': [
        'Platform.Cocoa/Base.lproj/MainMenu.xib',
        'Platform.Cocoa/English.lproj/InfoPlist.strings',
        'Platform.Cocoa/Images.xcassets/',
        'Content/',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': 'Platform.Cocoa/Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'LD_RUNPATH_SEARCH_PATHS': [
          '$(inherited)',
          '@executable_path/../Frameworks',
        ],
      },
    },
  ],
}
