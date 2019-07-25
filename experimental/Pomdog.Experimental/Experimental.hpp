// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Actions/Action.hpp"
#include "Actions/MoveToAction.hpp"
#include "Actions/RemoveActorAction.hpp"
#include "Actions/RotateToAction.hpp"
#include "Actions/ScaleToAction.hpp"
#include "Actions/SequenceAction.hpp"
#include "Actions/TintToAction.hpp"

#include "Compositing/RenderLayer.hpp"
#include "Compositing/RenderLayerCompositor.hpp"

#include "Gameplay/Component.hpp"
#include "Gameplay/Entity.hpp"
#include "Gameplay/EntityContext.hpp"
#include "Gameplay/EntityID.hpp"
#include "Gameplay/EntityManager.hpp"
#include "Gameplay/Scene.hpp"

#include "Gameplay2D/ActorComponent.hpp"
#include "Gameplay2D/Animator.hpp"
#include "Gameplay2D/CameraComponent.hpp"
#include "Gameplay2D/Collider2D.hpp"
#include "Gameplay2D/GameLevel.hpp"
#include "Gameplay2D/GraphicsComponent.hpp"
#include "Gameplay2D/PrimitiveRenderable.hpp"
#include "Gameplay2D/Simple2DGameEngine.hpp"
#include "Gameplay2D/SpriteRenderable.hpp"
#include "Gameplay2D/TextRenderable.hpp"
#include "Gameplay2D/Transform.hpp"

//#include "Graphics/SkinnedEffect.hpp"
//#include "Graphics/SpriteLine.hpp"
#include "Graphics/SpriteSortMode.hpp"

//#include "InGameEditor/InGameEditor.hpp"

#include "Skeletal2D/AnimationBlendInput.hpp"
#include "Skeletal2D/AnimationBlendInputType.hpp"
#include "Skeletal2D/AnimationClip.hpp"
#include "Skeletal2D/AnimationGraph.hpp"
#include "Skeletal2D/AnimationNode.hpp"
#include "Skeletal2D/AnimationState.hpp"
#include "Skeletal2D/AnimationSystem.hpp"
#include "Skeletal2D/AnimationTimeInterval.hpp"
#include "Skeletal2D/AnimationTrack.hpp"
#include "Skeletal2D/CompressedFloat.hpp"
#include "Skeletal2D/Joint.hpp"
#include "Skeletal2D/JointIndex.hpp"
#include "Skeletal2D/JointPose.hpp"
#include "Skeletal2D/RigidSlot.hpp"
#include "Skeletal2D/RotationTrack.hpp"
#include "Skeletal2D/ScaleTrack.hpp"
#include "Skeletal2D/Skeleton.hpp"
#include "Skeletal2D/SkeletonHelper.hpp"
#include "Skeletal2D/SkeletonPose.hpp"
#include "Skeletal2D/SkeletonTransform.hpp"
#include "Skeletal2D/Skin.hpp"
#include "Skeletal2D/SkinnedMesh.hpp"
#include "Skeletal2D/SkinnedMeshPart.hpp"
#include "Skeletal2D/SkinnedVertex.hpp"
#include "Skeletal2D/SpriteAnimationTrack.hpp"
#include "Skeletal2D/TranslationTrack.hpp"

#include "Spine/AnimationGraphBuilder.hpp"
#include "Spine/AnimationLoader.hpp"
#include "Spine/SkeletonDesc.hpp"
#include "Spine/SkeletonDescLoader.hpp"
#include "Spine/SkeletonLoader.hpp"
#include "Spine/SkinLoader.hpp"
#include "Spine/SkinnedMeshLoader.hpp"
#include "Spine/SpriteAnimationLoader.hpp"

#include "Rendering/RenderCommand.hpp"
#include "Rendering/Renderer.hpp"

#include "UI/DebugNavigator.hpp"
#include "UI/DisclosureTriangleButton.hpp"
#include "UI/DrawingContext.hpp"
#include "UI/FontSize.hpp"
#include "UI/FontWeight.hpp"
#include "UI/HorizontalAlignment.hpp"
#include "UI/HorizontalLayout.hpp"
#include "UI/HorizontalLine.hpp"
#include "UI/PointerEventType.hpp"
#include "UI/PointerPoint.hpp"
#include "UI/ScenePanel.hpp"
#include "UI/Slider.hpp"
#include "UI/StackPanel.hpp"
#include "UI/TextBlock.hpp"
#include "UI/Thickness.hpp"
#include "UI/ToggleSwitch.hpp"
#include "UI/TreeView.hpp"
#include "UI/UIElement.hpp"
#include "UI/UIElementHierarchy.hpp"
#include "UI/UIEventDispatcher.hpp"
#include "UI/UIHelper.hpp"
#include "UI/VerticalAlignment.hpp"
#include "UI/VerticalLayout.hpp"
