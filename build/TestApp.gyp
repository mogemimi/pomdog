# Examples(Xcode):
# gyp build/TestApp.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
#
# Examples(MSVS 2013):
# gyp build/TestApp.gyp --depth=. -f msvs -G msvs_version=2013 --generator-output=./build.msvc/

{
  'includes': ['common.gypi'],
  'make_global_settings': [
    ['CXX','/usr/bin/clang++'],
    ['LINK','/usr/bin/clang++'],
  ],
  'target_defaults': {
    'include_dirs': [
      '../include',
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
      'MACOSX_DEPLOYMENT_TARGET': '10.9', # OS X Deployment Target: 10.9
      'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
    },
  },
  'targets': [
    {
      'target_name': 'testapp',
      'product_name': 'TestApp',
      'type': 'executable',
      'mac_bundle': 1,
      'include_dirs': [
        '../include',
      ],
      'sources': [
        '../test/TestApp/SpriteRenderer.cpp',
        '../test/TestApp/SpriteRenderer.hpp',
        '../test/TestApp/TestAppGame.cpp',
        '../test/TestApp/TestAppGame.hpp',
      ],
      'conditions': [
        ['component == "shared_library"', {
          'dependencies': [
            'pomdog.gyp:pomdog-shared',
          ],
          'defines': [
            'POMDOG_USING_LIBRARY_EXPORTS=1',
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
            '../test/TestApp/Platform.Cocoa/main.mm',
            '../test/TestApp/Platform.Cocoa/TestApp-Prefix.pch',
            '../test/TestApp/Platform.Cocoa/AppDelegate.h',
            '../test/TestApp/Platform.Cocoa/AppDelegate.mm',
          ],
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
              '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
            ],
          },
          'actions': [
            {
              'action_name': 'XcodeResourcesUpdater',
              'description': 'Always update resources on Xcode',
              'inputs': [],
              'outputs': [],
              'action': ['./update-bundle-resources.sh', '${SOURCE_ROOT}/../test/TestApp/Content/.'],
            },
          ],
        }], # OS == "mac"
      ],
      'mac_bundle_resources': [
        '../test/TestApp/Content/',
        '../test/TestApp/Platform.Cocoa/Base.lproj/MainMenu.xib',
        '../test/TestApp/Platform.Cocoa/English.lproj/InfoPlist.strings',
        #'../test/TestApp/Platform.Cocoa/Images.xcassets/',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': '../test/TestApp/Platform.Cocoa/TestApp-Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
      },
    },
  ],# "targets"
}
