# Examples(Xcode):
# gyp build/TestApp.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
#
# Examples(MSVS 2013):
# gyp build/TestApp.gyp --depth=. -f msvs -G msvs_version=2013 --generator-output=./build.msvc/

{
  'includes': ['common.gypi', 'experimental.gypi'],
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
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++1y',
      'MACOSX_DEPLOYMENT_TARGET': '10.9', # OS X Deployment Target: 10.9
      'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
	  #'SKIP_INSTALL': 'NO',
      # Warnings:
      'CLANG_WARN_EMPTY_BODY': 'YES',
      'GCC_WARN_64_TO_32_BIT_CONVERSION': 'YES',
      'GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS': 'YES',
      'GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS': 'YES',
      'GCC_WARN_ABOUT_MISSING_NEWLINE': 'YES',
      'GCC_WARN_ABOUT_RETURN_TYPE': 'YES',
      'GCC_WARN_CHECK_SWITCH_STATEMENTS': 'YES',
      'GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS': 'YES',
      #'GCC_WARN_INITIALIZER_NOT_FULLY_BRACKETED': 'YES',
      'GCC_WARN_MISSING_PARENTHESES': 'YES',
      'GCC_WARN_NON_VIRTUAL_DESTRUCTOR': 'YES',
      'GCC_WARN_SHADOW': 'YES',
      'GCC_WARN_SIGN_COMPARE': 'YES',
      'GCC_WARN_TYPECHECK_CALLS_TO_PRINTF': 'YES',
      'GCC_WARN_UNINITIALIZED_AUTOS': 'YES',
      'GCC_WARN_UNKNOWN_PRAGMAS': 'YES',
      'GCC_WARN_UNUSED_FUNCTION': 'YES',
      'GCC_WARN_UNUSED_LABEL': 'YES',
      'GCC_WARN_UNUSED_VALUE': 'YES',
      'GCC_WARN_UNUSED_VARIABLE': 'YES',
      'GCC_TREAT_WARNINGS_AS_ERRORS': 'YES',
    },
  },
  'targets': [
    {
      'target_name': 'testapp',
      'product_name': 'TestApp',
      'type': 'executable',
      'mac_bundle': 1,
      'dependencies': [
        '../third_party/lua/lua.gyp:lua_static',
      ],
      'include_dirs': [
        '../include',
        '../experimental',
        '../third_party/rapidjson/include',
        '../third_party/lua/src',
      ],
      'sources': [
        '<@(pomdog_experimental_2d_sources)',
        '../test/TestApp/Utilities/LogSkeletalInfo.cpp',
        '../test/TestApp/Utilities/LogSkeletalInfo.hpp',
        '../test/TestApp/Utilities/SandboxHelper.hpp',
        '../test/TestApp/Levels/GunShootingLevel.cpp',
        '../test/TestApp/Levels/GunShootingLevel.hpp',
        '../test/TestApp/GrassBlendingGame.cpp',
        '../test/TestApp/GrassBlendingGame.hpp',
        '../test/TestApp/LightningTestGame.cpp',
        '../test/TestApp/LightningTestGame.hpp',
        '../test/TestApp/MaidBeamGame.cpp',
        '../test/TestApp/MaidBeamGame.hpp',
        '../test/TestApp/MaidChanGame.cpp',
        '../test/TestApp/MaidChanGame.hpp',
        '../test/TestApp/ParticleTestGame.cpp',
        '../test/TestApp/ParticleTestGame.hpp',
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
