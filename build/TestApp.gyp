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
      'include_dirs': [
        '../include',
        '../third_party/rapidjson/include',
      ],
      'sources': [
        '../test/TestApp/CameraView2D.cpp',
        '../test/TestApp/CameraView2D.hpp',
        '../test/TestApp/FXAA.cpp',
        '../test/TestApp/FXAA.hpp',
        '../test/TestApp/Skeletal2D/AnimationClip.cpp',
        '../test/TestApp/Skeletal2D/AnimationClip.hpp',
        '../test/TestApp/Skeletal2D/AnimationLoader.cpp',
        '../test/TestApp/Skeletal2D/AnimationLoader.hpp',
        '../test/TestApp/Skeletal2D/AnimationTimer.cpp',
        '../test/TestApp/Skeletal2D/AnimationTimer.hpp',
        '../test/TestApp/Skeletal2D/AnimationTrack.cpp',
        '../test/TestApp/Skeletal2D/AnimationTrack.hpp',
        '../test/TestApp/Skeletal2D/CompressedFloat.hpp',
        '../test/TestApp/Skeletal2D/Joint.hpp',
        '../test/TestApp/Skeletal2D/JointIndex.hpp',
        '../test/TestApp/Skeletal2D/JointPose.hpp',
        '../test/TestApp/Skeletal2D/RotationTrack.hpp',
        '../test/TestApp/Skeletal2D/ScaleTrack.hpp',
        '../test/TestApp/Skeletal2D/Skeleton.cpp',
        '../test/TestApp/Skeletal2D/Skeleton.hpp',
        '../test/TestApp/Skeletal2D/SkeletonDesc.hpp',
        '../test/TestApp/Skeletal2D/SkeletonDescLoader.cpp',
        '../test/TestApp/Skeletal2D/SkeletonDescLoader.hpp',
        '../test/TestApp/Skeletal2D/SkeletonLoader.cpp',
        '../test/TestApp/Skeletal2D/SkeletonLoader.hpp',
        '../test/TestApp/Skeletal2D/SkeletonPose.hpp',
        '../test/TestApp/Skeletal2D/Skin.cpp',
        '../test/TestApp/Skeletal2D/Skin.hpp',
        '../test/TestApp/Skeletal2D/SkinLoader.cpp',
        '../test/TestApp/Skeletal2D/SkinLoader.hpp',
        '../test/TestApp/Skeletal2D/SpriteAnimationLoader.cpp',
        '../test/TestApp/Skeletal2D/SpriteAnimationLoader.hpp',
        '../test/TestApp/Skeletal2D/SpriteAnimationTrack.cpp',
        '../test/TestApp/Skeletal2D/SpriteAnimationTrack.hpp',
        '../test/TestApp/Skeletal2D/TextureAtlas.hpp',
        '../test/TestApp/Skeletal2D/TextureAtlasLoader.cpp',
        '../test/TestApp/Skeletal2D/TextureAtlasLoader.hpp',
        '../test/TestApp/Skeletal2D/TranslationTrack.hpp',
        '../test/TestApp/PrimitiveAxes.cpp',
        '../test/TestApp/PrimitiveAxes.hpp',
        '../test/TestApp/PrimitiveGrid.cpp',
        '../test/TestApp/PrimitiveGrid.hpp',
        '../test/TestApp/SpriteBatch.cpp',
        '../test/TestApp/SpriteBatch.hpp',
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
