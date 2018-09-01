cmake_minimum_required(VERSION 3.10)
project(Pomdog CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_DIR "../..")
set(POMDOG_EXPERIMENTAL_DIR "../../experimental/Pomdog.Experimental")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

source_group(Actions                  REGULAR_EXPRESSION Pomdog.Experimental/Actions/*)
source_group(Gameplay                 REGULAR_EXPRESSION Pomdog.Experimental/Gameplay/*)
source_group(Gameplay\\detail         REGULAR_EXPRESSION Pomdog.Experimental/Gameplay/detail/*)
source_group(Gameplay2D               REGULAR_EXPRESSION Pomdog.Experimental/Gameplay2D/*)
source_group(Graphics                 REGULAR_EXPRESSION Pomdog.Experimental/Graphics/*)
source_group(ImageEffects             REGULAR_EXPRESSION Pomdog.Experimental/ImageEffects/*)
source_group(InGameEditor             REGULAR_EXPRESSION Pomdog.Experimental/InGameEditor/*)
source_group(Particle2D               REGULAR_EXPRESSION Pomdog.Experimental/Particle2D/*)
source_group(Particle2D\\detail       REGULAR_EXPRESSION Pomdog.Experimental/Particle2D/detail/*)
source_group(Skeletal2D               REGULAR_EXPRESSION Pomdog.Experimental/Skeletal2D/*)
source_group(Skeletal2D\\detail       REGULAR_EXPRESSION Pomdog.Experimental/Skeletal2D/detail/*)
source_group(Spine                    REGULAR_EXPRESSION Pomdog.Experimental/Spine/*)
source_group(Rendering                REGULAR_EXPRESSION Pomdog.Experimental/Rendering/*)
source_group(Rendering\\Commands      REGULAR_EXPRESSION Pomdog.Experimental/Rendering/Commands/*)
source_group(Rendering\\Processors    REGULAR_EXPRESSION Pomdog.Experimental/Rendering/Processors/*)
source_group(UI                       REGULAR_EXPRESSION Pomdog.Experimental/UI/*)
source_group(UI\\detail               REGULAR_EXPRESSION Pomdog.Experimental/UI/detail/*)
source_group(Utility                  REGULAR_EXPRESSION Pomdog.Experimental/Utility/*)

add_library(pomdog_experimental STATIC
  ${POMDOG_EXPERIMENTAL_DIR}/Actions/Action.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Actions/MoveToAction.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Actions/RemoveActorAction.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Actions/RotateToAction.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Actions/ScaleToAction.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Actions/SequenceAction.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Actions/TintToAction.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Actions/detail/TemporalAction.hpp

  # ${POMDOG_EXPERIMENTAL_DIR}/Compositing/RenderLayer.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Compositing/RenderLayer.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Compositing/RenderLayerCompositor.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Compositing/RenderLayerCompositor.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/Component.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/Entity.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/Entity.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/EntityContext.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/EntityContext.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/EntityID.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/EntityManager.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/EntityManager.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/Scene.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/Scene.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/detail/ComponentTypeIndex.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay/detail/ComponentTypeIndex.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/ActorComponent.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/ActorComponent.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/Animator.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/Animator.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/BeamRenderable.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/BeamRenderable.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/CameraComponent.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/CameraComponent.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/Collider2D.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/Collider2D.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/GameLevel.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/GraphicsComponent.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/GraphicsComponent.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/HierarchyNode.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/HierarchyNode.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/ParticleRenderable.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/ParticleRenderable.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/PrimitiveRenderable.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/PrimitiveRenderable.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/RectangleRenderable.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/RectangleRenderable.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/Simple2DGameEngine.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/Simple2DGameEngine.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/SkinnedMeshRenderable.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/SkinnedMeshRenderable.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/SpriteRenderable.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/SpriteRenderable.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/TextRenderable.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/TextRenderable.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/Transform.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Gameplay2D/Transform.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/Graphics/ScreenQuad.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Graphics/ScreenQuad.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Graphics/SkinnedEffect.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Graphics/SkinnedEffect.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Graphics/SpriteLine.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Graphics/SpriteLine.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Graphics/SpriteSortMode.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Graphics/SpriteRenderer.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Graphics/SpriteRenderer.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/ChromaticAberration.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/ChromaticAberration.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/FishEyeEffect.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/FishEyeEffect.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/FXAA.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/FXAA.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/GrayscaleEffect.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/GrayscaleEffect.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/ImageEffectBase.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/PostProcessCompositor.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/PostProcessCompositor.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/RetroCrtEffect.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/RetroCrtEffect.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/SepiaToneEffect.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/SepiaToneEffect.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/VignetteEffect.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/ImageEffects/VignetteEffect.hpp

  # ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/InGameEditor.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/InGameEditor.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/detail/EditorBackground.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/detail/EditorBackground.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/detail/EditorColorScheme.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/detail/PrimitiveAxes.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/detail/PrimitiveAxes.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/detail/PrimitiveGrid.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/InGameEditor/detail/PrimitiveGrid.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/Beam.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/BeamBranching.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/BeamEmitter.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/BeamSystem.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/BeamSystem.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/Particle.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/ParticleClip.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/ParticleClip.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/ParticleEmitter.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/ParticleLoader.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/ParticleLoader.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/ParticleSystem.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/ParticleSystem.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleCurveKey.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleCurveLerp.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleEmitterShape.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleEmitterShapeBox.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleEmitterShapeSector.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleParameter.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleParameterConstant.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleParameterCurve.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleParameterRandom.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Particle2D/detail/ParticleParameterRandomCurves.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationBlendInput.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationBlendInputType.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationClip.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationClip.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationGraph.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationNode.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationState.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationState.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationSystem.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationSystem.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationTimeInterval.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationTrack.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/AnimationTrack.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/CompressedFloat.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/Joint.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/JointIndex.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/JointPose.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/RigidSlot.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/RotationTrack.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/ScaleTrack.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/Skeleton.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/Skeleton.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SkeletonHelper.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SkeletonHelper.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SkeletonPose.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SkeletonPose.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SkeletonTransform.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/Skin.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/Skin.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SkinnedMesh.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SkinnedMeshPart.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SkinnedVertex.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SpriteAnimationTrack.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/SpriteAnimationTrack.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/TranslationTrack.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationAdditiveNode.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationAdditiveNode.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationClipNode.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationClipNode.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationGraphWeight.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationGraphWeight.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationGraphWeightCollection.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationGraphWeightCollection.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationKeyHelper.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationLerpNode.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationLerpNode.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationTimer.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/AnimationTimer.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/WeightBlendingHelper.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Skeletal2D/detail/WeightBlendingHelper.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/Spine/AnimationGraphBuilder.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/AnimationGraphBuilder.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/AnimationLoader.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/AnimationLoader.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkeletonDesc.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkeletonDescLoader.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkeletonDescLoader.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkeletonLoader.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkeletonLoader.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkinLoader.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkinLoader.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkinnedMeshLoader.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SkinnedMeshLoader.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SpriteAnimationLoader.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Spine/SpriteAnimationLoader.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/RenderCommand.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/RenderCommandProcessor.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Renderer.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Renderer.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/ParticleBatchCommand.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/ParticleBatchCommand.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/PrimitiveCommand.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/PrimitiveCommand.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/PrimitivePolygonCommand.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/PrimitivePolygonCommand.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/SkinnedMeshCommand.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/SkinnedMeshCommand.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/SpriteBatchCommand.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Commands/SpriteBatchCommand.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/ParticleBatchCommandProcessor.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/ParticleBatchCommandProcessor.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/PrimitiveCommandProcessor.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/PrimitiveCommandProcessor.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/PrimitivePolygonCommandProcessor.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/PrimitivePolygonCommandProcessor.hpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/SkinnedMeshCommandProcessor.cpp
  # ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/SkinnedMeshCommandProcessor.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/SpriteBatchCommandProcessor.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Rendering/Processors/SpriteBatchCommandProcessor.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/UI/DebugNavigator.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/DebugNavigator.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/DisclosureTriangleButton.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/DisclosureTriangleButton.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/DrawingContext.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/DrawingContext.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/FontSize.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/FontWeight.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/HorizontalAlignment.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/HorizontalLayout.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/HorizontalLayout.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/HorizontalLine.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/HorizontalLine.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/PointerEventType.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/PointerPoint.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/ScenePanel.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/ScenePanel.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/Slider.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/Slider.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/StackPanel.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/StackPanel.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/TextBlock.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/TextBlock.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/Thickness.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/ToggleSwitch.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/ToggleSwitch.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/TreeView.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/TreeView.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/UIElement.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/UIElement.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/UIElementHierarchy.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/UIElementHierarchy.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/UIEventDispatcher.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/UIEventDispatcher.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/UIHelper.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/VerticalAlignment.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/VerticalLayout.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/VerticalLayout.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/detail/SubscribeRequestDispatcher.hpp
  ${POMDOG_EXPERIMENTAL_DIR}/UI/detail/UIEventConnection.hpp

  ${POMDOG_EXPERIMENTAL_DIR}/Utility/UserPreferences.cpp
  ${POMDOG_EXPERIMENTAL_DIR}/Utility/UserPreferences.hpp
)

target_include_directories(pomdog_experimental PRIVATE
  ${POMDOG_DIR}/experimental
  ${POMDOG_DIR}/include
  ${POMDOG_DIR}/dependencies/rapidjson/include
)

target_compile_definitions(pomdog_experimental PRIVATE
  $<$<CONFIG:DEBUG>:_DEBUG;DEBUG=1>
  $<$<CONFIG:RELEASE>:NDEBUG>

  # On Windows
  $<$<PLATFORM_ID:Windows>:
    _WIN32_WINNT=0x0602 # Windows 8 or later
    WIN32_LEAN_AND_MEAN
    NOMINMAX
  >
)

target_compile_options(pomdog_experimental PRIVATE
  # Clang and GCC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  # MSVC
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4
    $<$<CONFIG:DEBUG>:/Od;/MTd>
    $<$<CONFIG:RELEASE>:/O2;/Ob2;/MT>
  >
)

set_target_properties(
  pomdog_experimental PROPERTIES
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
  XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
  XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

  XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
  XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17"
  XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
  XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.11"

  # Warnings (Clang)
  XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_UNREACHABLE_CODE "YES"

  # Warnings (GCC and Clang)
  XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ABOUT_MISSING_NEWLINE "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ABOUT_RETURN_TYPE "YES_ERROR"
  XCODE_ATTRIBUTE_GCC_WARN_CHECK_SWITCH_STATEMENTS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_MISSING_PARENTHESES "YES"
  XCODE_ATTRIBUTE_GCC_WARN_NON_VIRTUAL_DESTRUCTOR "YES"
  XCODE_ATTRIBUTE_GCC_WARN_SHADOW "YES"
  XCODE_ATTRIBUTE_GCC_WARN_SIGN_COMPARE "YES"
  XCODE_ATTRIBUTE_GCC_WARN_TYPECHECK_CALLS_TO_PRINTF "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNINITIALIZED_AUTOS "YES_AGGRESSIVE"
  XCODE_ATTRIBUTE_GCC_WARN_UNKNOWN_PRAGMAS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNUSED_FUNCTION "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNUSED_LABEL "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VALUE "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VARIABLE "YES"

  # Warnings - Objective-C
  XCODE_ATTRIBUTE_CLANG_WARN_DIRECT_OBJC_ISA_USAGE "YES_ERROR"
  XCODE_ATTRIBUTE_CLANG_WARN__DUPLICATE_METHOD_MATCH "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ALLOW_INCOMPLETE_PROTOCOL "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNDECLARED_SELECTOR "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_OBJC_ROOT_CLASS "YES_ERROR"

  # Warning Policies
  # XCODE_ATTRIBUTE_GCC_TREAT_WARNINGS_AS_ERRORS "YES"

  # Symbols
  XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
  XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES" # -fvisibility-inlines-hidden
  XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES" # -fvisibility=hidden
)

if(NOT POMDOG_BUILD_TARGET_ALL)
  add_subdirectory(${POMDOG_DIR}/build/pomdog ${CMAKE_CURRENT_BINARY_DIR}/pomdog_build)
endif()
target_link_libraries(pomdog_experimental INTERFACE pomdog_static)
