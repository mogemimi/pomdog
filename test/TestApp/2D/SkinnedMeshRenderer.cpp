//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkinnedMeshRenderer.hpp"
#include "RenderCommand.hpp"
#include "RenderQueue.hpp"
#include "../Skeletal2D/AnimationTrack.hpp"
#include "../Skeletal2D/SkeletonHelper.hpp"
#include "../Skeletal2D/SkinnedMesh.hpp"
#include "../Spine/SkeletonDescLoader.hpp"
#include "../Spine/AnimationLoader.hpp"
#include "../Spine/SkeletonLoader.hpp"
#include "../Spine/SkinLoader.hpp"
#include "../Spine/SpriteAnimationLoader.hpp"
#include "../Spine/SkinnedMeshLoader.hpp"
#include "../TexturePacker/TextureAtlasLoader.hpp"
#include "../Utilities/LogSkeletalInfo.hpp"
#include "../Skeletal2D/AnimationNode.hpp"
#include "../Skeletal2D/AnimationAdditiveNode.hpp"
#include "../Skeletal2D/AnimationLerpNode.hpp"
#include "../Skeletal2D/AnimationClipNode.hpp"

namespace Pomdog {
namespace {

//static Matrix3x2 CreateTransformMatrix3x2(Transform2D const& transform)
//{
//	return Matrix3x2::CreateScale(transform.Scale)
//		* Matrix3x2::CreateRotation(transform.Rotation)
//		* Matrix3x2::CreateTranslation(transform.Position);
//}

static Matrix4x4 CreateTransformMatrix4x4(Transform2D const& transform)
{
	return Matrix4x4::CreateScale({transform.Scale.X, transform.Scale.Y, 1.0f})
		* Matrix4x4::CreateRotationZ(transform.Rotation)
		* Matrix4x4::CreateTranslation({transform.Position.X, transform.Position.Y, 1.0f});
}


}// unnamed namespace
//-----------------------------------------------------------------------
struct SkinnedMeshCommand: public RenderCommand
{
	float ZOrder() const override
	{
		return zOrder;
	}

	void Execute(std::shared_ptr<GraphicsContext> const& graphicsContext) override
	{
		//auto transform = gameObject->Component<Transform2D>();
		//renderer->DrawSkeleton(polygonBatch, modelViewProjection);
		renderer->DrawSkinnedMesh(graphicsContext, modelViewProjection);
	}

	Matrix4x4 modelViewProjection;
	SkinnedMeshRenderer* renderer = nullptr;
	float zOrder;
};
//-----------------------------------------------------------------------
SkinnedMeshRenderer::SkinnedMeshRenderer()
	: zOrder(0)
	, isVisible(true)
{
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderer::Load(std::shared_ptr<GraphicsDevice> const& graphicsDevice, std::shared_ptr<AssetManager> const& assets)
{
	auto textureAtlas = assets->Load<Details::TexturePacker::TextureAtlas>("MaidGun/MaidGun.atlas");
	auto skeletonDesc = assets->Load<Details::Skeletal2D::SkeletonDesc>("MaidGun/MaidGun.json");
	maidTexture = assets->Load<Texture2D>("MaidGun/MaidGun.png");
	
	TestApp::LogSkeletalInfo(textureAtlas, skeletonDesc);
	
	maidSkeleton = std::make_shared<Skeleton>(Details::Skeletal2D::CreateSkeleton(skeletonDesc.Bones));
	maidSkeletonPose = std::make_shared<SkeletonPose>(SkeletonPose::CreateBindPose(*maidSkeleton));
	auto animationClip = std::make_shared<AnimationClip>(Details::Skeletal2D::CreateAnimationClip(skeletonDesc, "Run"));
	maidAnimationState = std::make_shared<AnimationState>(animationClip, 1.0f, true);
	maidAnimationClipIdle = std::make_shared<AnimationClip>(Details::Skeletal2D::CreateAnimationClip(skeletonDesc, "RunNoShot"));
	
	maidSkin = Details::Skeletal2D::CreateSkin(skeletonDesc, textureAtlas, "default");
	maidSpriteAnimationTracks = Details::Skeletal2D::CreateSpriteAnimationTrack(skeletonDesc, textureAtlas, "Run");
	
	animationSystem.Add(maidAnimationState, maidSkeleton, maidSkeletonPose);

	maidGlobalPose = SkeletonHelper::ToGlobalPose(*maidSkeleton, *maidSkeletonPose);

	// NOTE: for Skinning
	auto bindPose = SkeletonPose::CreateBindPose(*maidSkeleton);
	maidSkinnedMesh = Details::Skeletal2D::CreateSkinnedMesh(graphicsDevice,
		SkeletonHelper::ToGlobalPose(*maidSkeleton, bindPose),
		skeletonDesc, textureAtlas,
		Vector2(maidTexture->Width(), maidTexture->Height()), "default");
	maidSkinningEffect = assets->Load<EffectPass>("Effects/SkinningSpriteEffect");
	maidInputLayout = std::make_shared<InputLayout>(graphicsDevice, maidSkinningEffect);
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderer::Update(GameClock const& clock)
{
	{
		animationSystem.Update(clock);
		
		SkeletonHelper::ToGlobalPose(*maidSkeleton, *maidSkeletonPose, maidGlobalPose);
	
		{
			///@note Test code for animation blending
			auto clipNode1 = MakeUnique<AnimationClipNode>(maidAnimationState->Clip());
			auto clipNode2 = MakeUnique<AnimationClipNode>(maidAnimationClipIdle);
		
			auto lerpNode = MakeUnique<AnimationLerpNode>(std::move(clipNode1), std::move(clipNode2));
			//lerpNode->Weight(slider2->Value());
			lerpNode->Weight(0.2f);
			
			lerpNode->Calculate(maidAnimationState->Time(), *maidSkeleton, *maidSkeletonPose);
			
			SkeletonHelper::ToGlobalPose(*maidSkeleton, *maidSkeletonPose, maidGlobalPose);
		}
	}
	{
		maidAnimationTimer.Update(clock.FrameDuration());
		if (maidAnimationTimer.Time() > maidAnimationState->Length()) {
			maidAnimationTimer.Reset();
		}
	}
	{
		for (auto & track: maidSpriteAnimationTracks)
		{
			track.Apply(maidSkin, maidAnimationTimer.Time());
		}
	}
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderer::Visit(GameObject & gameObject, RenderQueue & renderQueue,
	Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix)
{
	///@todo Not implemented
	
	if (!isVisible) {
		return;
	}

	auto command = MakeUnique<SkinnedMeshCommand>();
	command->zOrder = zOrder;
	command->renderer = this;
	command->modelViewProjection = viewMatrix * projectionMatrix;
	
	if (auto transform = gameObject.Component<Transform2D>())
	{
		command->modelViewProjection = CreateTransformMatrix4x4(*transform)
			* command->modelViewProjection;
	}
	
	renderQueue.PushBack(std::move(command));
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderer::DrawSkeleton(std::unique_ptr<PolygonBatch> const& polygonBatch,
	Matrix4x4 const& modelViewProjection)
{
//		if (!toggleSwitch3->IsOn()) {
//			return;
//		}

	POMDOG_ASSERT(polygonBatch);
	polygonBatch->Begin(modelViewProjection);

	auto const& globalPoses = maidGlobalPose;
	
	Color boneColor {160, 160, 160, 255};

	for (auto & joint: *maidSkeleton)
	{
		auto & matrix = globalPoses[*joint.Index];

		if (maidSkeleton->Root().Index != joint.Index)
		{
			polygonBatch->DrawTriangle(
				Vector2::Transform({1.7f, -4.7f}, matrix),
				Vector2::Transform({1.7f, 4.7f}, matrix),
				Vector2::Transform({25, 0}, matrix), boneColor);
		}
		
		auto center = Vector2::Transform(Vector2::Zero, matrix);
		polygonBatch->DrawCircle(center, 5.0f, boneColor, 18);
		polygonBatch->DrawCircle(center, 3.0f, Color::White, 13);
	}
	
	polygonBatch->End();
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderer::DrawSkinnedMesh(std::shared_ptr<GraphicsContext> const& graphicsContext,
	Matrix4x4 const& modelViewProjection)
{
	{
		maidSkinningEffect->Parameters("Constants")->SetValue(Matrix4x4::Transpose(modelViewProjection));

		struct MatrixPalette {
			std::array<Vector4, 64> matrixPalette1;
			std::array<Vector4, 64> matrixPalette2;
		};
		MatrixPalette matrixPalette;
		
		std::array<Matrix3x2, 64> matrices;

		for (auto & joint: *maidSkeleton)
		{
			POMDOG_ASSERT(joint.Index);
			POMDOG_ASSERT(*joint.Index < matrices.size());
			POMDOG_ASSERT(*joint.Index < maidGlobalPose.size());
			matrices[*joint.Index] = joint.InverseBindPose * maidGlobalPose[*joint.Index];
		}

		for (std::size_t i = 0; i < matrices.size(); ++i) {
			matrixPalette.matrixPalette1[i].X = matrices[i](0, 0);
			matrixPalette.matrixPalette1[i].Y = matrices[i](0, 1);
			matrixPalette.matrixPalette1[i].Z = matrices[i](1, 0);
			matrixPalette.matrixPalette1[i].W = matrices[i](1, 1);
			matrixPalette.matrixPalette2[i].X = matrices[i](2, 0);
			matrixPalette.matrixPalette2[i].Y = matrices[i](2, 1);
		}

		maidSkinningEffect->Parameters("SkinningConstants")->SetValue(matrixPalette);
	}
	
	//if (toggleSwitch2->IsOn())
	{
		graphicsContext->SetTexture(0, maidTexture);
		graphicsContext->SetInputLayout(maidInputLayout);
		graphicsContext->SetVertexBuffer(maidSkinnedMesh.VertexBuffer);
		maidSkinningEffect->Apply();
		graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList,
			maidSkinnedMesh.IndexBuffer, maidSkinnedMesh.IndexBuffer->IndexCount());
	}
	
//	if (toggleSwitch4->IsOn())
//	{
//		RasterizerDescription rasterizerDesc;
//		rasterizerDesc.FillMode = FillMode::WireFrame;
//		auto rasterizerState = std::make_shared<RasterizerState>(gameHost->GraphicsDevice(), rasterizerDesc);
//		
//		graphicsContext->SetRasterizerState(rasterizerState);
//		
//		graphicsContext->SetTexture(0, texture);
//		graphicsContext->SetInputLayout(maidInputLayout);
//		graphicsContext->SetVertexBuffer(maidSkinnedMesh.VertexBuffer);
//		maidSkinningEffect->Apply();
//		graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList,
//									 maidSkinnedMesh.IndexBuffer, maidSkinnedMesh.IndexBuffer->IndexCount());
//		
//		
//		graphicsContext->SetRasterizerState(RasterizerState::CreateCullCounterClockwise(gameHost->GraphicsDevice()));
//	}
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderer::SetZOrder(float zOrderIn)
{
	this->zOrder = zOrderIn;
}
//-----------------------------------------------------------------------
float SkinnedMeshRenderer::GetZOrder() const
{
	return zOrder;
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderer::SetVisible(bool isVisibleIn)
{
	this->isVisible = isVisibleIn;
}
//-----------------------------------------------------------------------
bool SkinnedMeshRenderer::IsVisible() const
{
	return isVisible;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
