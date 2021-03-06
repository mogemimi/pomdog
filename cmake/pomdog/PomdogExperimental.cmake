source_group(Experimental\\Async                    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Async/*")
source_group(Experimental\\ECS                      REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/ECS/*")
source_group(Experimental\\GLTF                     REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/GLTF/*")
source_group(Experimental\\Graphics                 REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Graphics/*")
source_group(Experimental\\GUI                      REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/GUI/*")
source_group(Experimental\\ImageEffects             REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/ImageEffects/*")
source_group(Experimental\\Image                    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Image/*")
source_group(Experimental\\MagicaVoxel              REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/MagicaVoxel/*")
source_group(Experimental\\Particles                REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Particles/*")
source_group(Experimental\\Particles\\EmitterShapes REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Particles/EmitterShapes/*")
source_group(Experimental\\Particles\\Parameters    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Particles/Parameters/*")
source_group(Experimental\\Random                   REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Random/*")
source_group(Experimental\\Reactive                 REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Reactive/*")
source_group(Experimental\\Skeletal2D               REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Skeletal2D/*")
source_group(Experimental\\Skeletal2D\\BlendTrees   REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Skeletal2D/BlendTrees/*")
source_group(Experimental\\Skeletal2D\\Tracks       REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Skeletal2D/Tracks/*")
source_group(Experimental\\Spine                    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Spine/*")
source_group(Experimental\\TexturePacker            REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/TexturePacker/*")
source_group(Experimental\\Tween                    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Tween/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Experimental/Async
    ${POMDOG_INC_DIR}/Experimental/Async/Helpers.hpp
    ${POMDOG_INC_DIR}/Experimental/Async/ImmediateScheduler.hpp
    ${POMDOG_SRC_DIR}/Experimental/Async/ImmediateScheduler.cpp
    ${POMDOG_INC_DIR}/Experimental/Async/QueuedScheduler.hpp
    ${POMDOG_SRC_DIR}/Experimental/Async/QueuedScheduler.cpp
    ${POMDOG_INC_DIR}/Experimental/Async/Scheduler.hpp
    ${POMDOG_SRC_DIR}/Experimental/Async/Scheduler.cpp
    ${POMDOG_INC_DIR}/Experimental/Async/Task.hpp
    ${POMDOG_SRC_DIR}/Experimental/Async/Task.cpp
)

target_sources(pomdog_static PRIVATE
    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/ECS
        ${POMDOG_INC_DIR}/Experimental/ECS/ComponentBuffer.hpp
        ${POMDOG_SRC_DIR}/Experimental/ECS/ComponentBuffer.cpp
        ${POMDOG_INC_DIR}/Experimental/ECS/ComponentType.hpp
        ${POMDOG_INC_DIR}/Experimental/ECS/ComponentTypeIndex.hpp
        ${POMDOG_SRC_DIR}/Experimental/ECS/ComponentTypeIndex.cpp
        ${POMDOG_INC_DIR}/Experimental/ECS/Entity.hpp
        ${POMDOG_SRC_DIR}/Experimental/ECS/Entity.cpp
        ${POMDOG_INC_DIR}/Experimental/ECS/EntityArchtype.hpp
        ${POMDOG_INC_DIR}/Experimental/ECS/EntityDesc.hpp
        ${POMDOG_INC_DIR}/Experimental/ECS/EntityManager.hpp
        ${POMDOG_SRC_DIR}/Experimental/ECS/EntityManager.cpp
        ${POMDOG_INC_DIR}/Experimental/ECS/EntityQuery.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/GLTF
        ${POMDOG_INC_DIR}/Experimental/GLTF/GLTF.hpp
        ${POMDOG_SRC_DIR}/Experimental/GLTF/GLTF.cpp
        ${POMDOG_INC_DIR}/Experimental/GLTF/GLTFLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/GLTF/GLTFLoader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/Graphics
        ${POMDOG_INC_DIR}/Experimental/Graphics/BasicEffect.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/BasicEffect.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/BillboardEffect.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/BillboardEffect.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/FontGlyph.hpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/LineBatch.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/LineBatch.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/PolylineBatch.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/PolylineBatch.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/PolygonShapeBuilder.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/PolygonShapeBuilder.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/PrimitiveBatch.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/PrimitiveBatch.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/SpriteBatch.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/SpriteBatch.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/SpriteFont.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/SpriteFont.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/SpriteFontLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/SpriteFontLoader.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/SpriteLine.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/SpriteLine.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/Texture2DView.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/Texture2DView.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/TrueTypeFont.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/TrueTypeFont.cpp
        ${POMDOG_INC_DIR}/Experimental/Graphics/TrueTypeFontLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Graphics/TrueTypeFontLoader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/GUI
        ${POMDOG_INC_DIR}/Experimental/GUI/CheckBox.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/CheckBox.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/ContextMenu.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/ContextMenu.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/ContextMenuItem.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/ContextMenuItem.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/DebugNavigator.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/DebugNavigator.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/DisclosureTriangleButton.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/DisclosureTriangleButton.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/DrawingContext.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/DrawingContext.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/FloatField.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/FloatField.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/FontSize.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/FontWeight.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/HierarchySortOrder.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/HorizontalAlignment.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/HorizontalLayout.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/HorizontalLayout.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/HorizontalLine.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/HorizontalLine.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/HorizontalSplitter.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/HorizontalSplitter.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/IntField.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/IntField.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/ListView.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/ListView.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/ListViewItem.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/ListViewItem.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/NumberField.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/NumberField.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/PointerEventType.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/PointerPoint.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/PopupMenu.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/PopupMenu.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/PushButton.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/PushButton.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/ScenePanel.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/ScenePanel.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/ScrollView.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/ScrollView.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/Slider.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/Slider.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/StackPanel.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/StackPanel.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/TextAlignment.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/TextBlock.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/TextBlock.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/TextEdit.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/TextEdit.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/TextField.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/TextField.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/Thickness.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/ToggleSwitch.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/ToggleSwitch.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/TreeView.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/TreeView.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/UIEventDispatcher.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/UIEventDispatcher.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/UIHelper.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/UIHelper.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/Vector3Field.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/Vector3Field.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/VerticalAlignment.hpp
        ${POMDOG_INC_DIR}/Experimental/GUI/VerticalLayout.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/VerticalLayout.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/VerticalScrollBar.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/VerticalScrollBar.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/Widget.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/Widget.cpp
        ${POMDOG_INC_DIR}/Experimental/GUI/WidgetHierarchy.hpp
        ${POMDOG_SRC_DIR}/Experimental/GUI/WidgetHierarchy.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/Image
        ${POMDOG_INC_DIR}/Experimental/Image/GIFImage.hpp
        ${POMDOG_INC_DIR}/Experimental/Image/GIFLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Image/GIFLoader.cpp
        ${POMDOG_INC_DIR}/Experimental/Image/Image.hpp
        ${POMDOG_SRC_DIR}/Experimental/Image/Image.cpp
        ${POMDOG_INC_DIR}/Experimental/Image/SVGLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Image/SVGLoader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/ImageEffects
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/ChromaticAberration.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/ChromaticAberration.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/FishEyeEffect.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/FishEyeEffect.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/FXAA.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/FXAA.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/GrayscaleEffect.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/GrayscaleEffect.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/ImageEffectBase.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/ImageEffectBase.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/PostProcessCompositor.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/PostProcessCompositor.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/RetroCrtEffect.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/RetroCrtEffect.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/ScreenQuad.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/ScreenQuad.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/SepiaToneEffect.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/SepiaToneEffect.cpp
        ${POMDOG_INC_DIR}/Experimental/ImageEffects/VignetteEffect.hpp
        ${POMDOG_SRC_DIR}/Experimental/ImageEffects/VignetteEffect.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/MagicaVoxel
        ${POMDOG_INC_DIR}/Experimental/MagicaVoxel/VoxModel.hpp
        ${POMDOG_INC_DIR}/Experimental/MagicaVoxel/VoxModelExporter.hpp
        ${POMDOG_SRC_DIR}/Experimental/MagicaVoxel/VoxModelExporter.cpp
        ${POMDOG_INC_DIR}/Experimental/MagicaVoxel/VoxModelLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/MagicaVoxel/VoxModelLoader.cpp
        ${POMDOG_SRC_DIR}/Experimental/MagicaVoxel/VoxChunkHeader.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/Particles
        ${POMDOG_INC_DIR}/Experimental/Particles/Beam.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/BeamBranching.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/BeamEmitter.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/BeamSystem.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/BeamSystem.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/Particle.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/ParticleClip.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/ParticleClipLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/ParticleClipLoader.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/ParticleSystem.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/ParticleSystem.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShape.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeBox.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeBox.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeCone.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeCone.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeHemisphere.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeHemisphere.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeSector.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeSector.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeSphere.hpp
        ${POMDOG_SRC_DIR}/Experimental/Particles/EmitterShapes/ParticleEmitterShapeSphere.cpp
        ${POMDOG_INC_DIR}/Experimental/Particles/Parameters/ParticleCurveKey.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/Parameters/ParticleCurveLerp.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/Parameters/ParticleParameter.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/Parameters/ParticleParameterConstant.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/Parameters/ParticleParameterCurve.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/Parameters/ParticleParameterRandom.hpp
        ${POMDOG_INC_DIR}/Experimental/Particles/Parameters/ParticleParameterRandomCurves.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/Random
        ${POMDOG_INC_DIR}/Experimental/Random/Xoroshiro128StarStar.hpp
        ${POMDOG_SRC_DIR}/Experimental/Random/Xoroshiro128StarStar.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/Reactive
        ${POMDOG_INC_DIR}/Experimental/Reactive/Observable.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/ObservableBase.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/ObservableBuilder.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Observer.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Subscriber.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/BufferOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/DebounceOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/DelayOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/DistinctOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/DoOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/FilterOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/FirstOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/LastOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/MapOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/MergeOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/ScanOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/SkipLastOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/SkipOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/TakeOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/TimeoutOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Operators/ZipOperator.hpp
        ${POMDOG_INC_DIR}/Experimental/Reactive/Subjects/KeyDownSubject.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/Skeletal2D
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/AnimationClip.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/AnimationClip.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/AnimationState.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/AnimationState.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/AnimationSystem.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/AnimationSystem.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/AnimationTimeInterval.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Animator.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/Animator.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/CompressedFloat.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Joint.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/JointIndex.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/JointPose.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/RigidSlot.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Skeleton.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/Skeleton.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/SkeletonHelper.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/SkeletonHelper.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/SkeletonPose.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/SkeletonPose.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Skin.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/Skin.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/SkinnedMesh.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/SkinnedMeshPart.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/SkinnedVertex.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationAdditiveNode.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationAdditiveNode.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationBlendInput.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationBlendInputType.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationClipNode.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationClipNode.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationCrossFadeNode.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationCrossFadeNode.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationGraph.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationGraphWeightCollection.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationGraphWeightCollection.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationLerpNode.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationLerpNode.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationNode.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/BlendTrees/AnimationNode.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/BlendTrees/WeightBlendingHelper.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/BlendTrees/WeightBlendingHelper.cpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/Tracks/AnimationKeyHelper.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Tracks/AnimationTrack.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/Tracks/AnimationTrack.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Tracks/RotationTrack.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Tracks/ScaleTrack.hpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Tracks/SpriteAnimationTrack.hpp
        ${POMDOG_SRC_DIR}/Experimental/Skeletal2D/Tracks/SpriteAnimationTrack.cpp
        ${POMDOG_INC_DIR}/Experimental/Skeletal2D/Tracks/TranslationTrack.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/Spine
        ${POMDOG_INC_DIR}/Experimental/Spine/AnimationGraphBuilder.hpp
        ${POMDOG_SRC_DIR}/Experimental/Spine/AnimationGraphBuilder.cpp
        ${POMDOG_INC_DIR}/Experimental/Spine/AnimationLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Spine/AnimationLoader.cpp
        ${POMDOG_INC_DIR}/Experimental/Spine/SkeletonDesc.hpp
        ${POMDOG_INC_DIR}/Experimental/Spine/SkeletonDescLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Spine/SkeletonDescLoader.cpp
        ${POMDOG_INC_DIR}/Experimental/Spine/SkeletonLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Spine/SkeletonLoader.cpp
        ${POMDOG_INC_DIR}/Experimental/Spine/SkinLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Spine/SkinLoader.cpp
        ${POMDOG_INC_DIR}/Experimental/Spine/SkinnedMeshLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/Spine/SkinnedMeshLoader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/TexturePacker
        ${POMDOG_INC_DIR}/Experimental/TexturePacker/TextureAtlas.hpp
        ${POMDOG_INC_DIR}/Experimental/TexturePacker/TextureAtlasGenerator.hpp
        ${POMDOG_SRC_DIR}/Experimental/TexturePacker/TextureAtlasGenerator.cpp
        ${POMDOG_INC_DIR}/Experimental/TexturePacker/TextureAtlasLoader.hpp
        ${POMDOG_SRC_DIR}/Experimental/TexturePacker/TextureAtlasLoader.cpp
        ${POMDOG_INC_DIR}/Experimental/TexturePacker/TextureRegion.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: Experimental/Tween
        ${POMDOG_INC_DIR}/Experimental/Tween/EasingHelper.hpp
        ${POMDOG_SRC_DIR}/Experimental/Tween/EasingHelper.cpp
    >
)
