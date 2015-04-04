{
  'includes': ['common.gypi'],
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
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++14',
      'MACOSX_DEPLOYMENT_TARGET': '10.9',
      'CLANG_CXX_LIBRARY': 'libc++',
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
      ['OS == "mac"', {
        'link_settings': {
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          ],
        },
      }], # OS == "mac"
    ],
  },
  'targets': [
    {
      'target_name': 'QuickStart',
      'product_name': 'QuickStart',
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
            '../Platform.Cocoa/AppDelegate.h',
            '../Platform.Cocoa/AppDelegate.mm',
            '../Platform.Cocoa/GameView.h',
            '../Platform.Cocoa/GameView.mm',
            '../Platform.Cocoa/GameViewController.h',
            '../Platform.Cocoa/GameViewController.mm',
          ],
          'actions': [
            {
              'action_name': 'XcodeResourcesUpdater',
              'description': 'Always update resources on Xcode',
              'inputs': [],
              'outputs': [],
              'action': ['./update-bundle-resources.sh', '${SOURCE_ROOT}/../Content/.'],
            },
          ],
        }], # OS == "mac"
      ],
      'mac_bundle_resources': [
        '../Platform.Cocoa/Base.lproj/MainMenu.xib',
        '../Platform.Cocoa/English.lproj/InfoPlist.strings',
        '../Platform.Cocoa/Images.xcassets/',
        '../Content/',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': '../Platform.Cocoa/Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
      },
    },
  ],
}
