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
#include "../Skeletal2D/SkeletonHelper.hpp"
#include "../Skeletal2D/SkinnedMesh.hpp"

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
struct SkinnedMeshRenderCommand: public RenderCommand
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
SkinnedMeshRenderer::SkinnedMeshRenderer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<AssetManager> const& assets,
	std::shared_ptr<Skeleton> const& skeletonIn,
	std::shared_ptr<SkeletonTransform> const& skeletonTransformIn,
	std::shared_ptr<SkinnedMesh> const& meshIn, std::shared_ptr<Texture2D> const& textureIn)
	: mesh(meshIn)
	, texture(textureIn)
	, skeleton(skeletonIn)
	, skeletonTransform(skeletonTransformIn)
	, zOrder(0)
	, isVisible(true)
{
	POMDOG_ASSERT(mesh);
	POMDOG_ASSERT(texture);
	POMDOG_ASSERT(skeleton);
	POMDOG_ASSERT(skeletonTransform);

	skinningEffect = assets->Load<EffectPass>("Effects/SkinningSpriteEffect");
	inputLayout = std::make_shared<InputLayout>(graphicsDevice, skinningEffect);
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderer::Visit(GameObject & gameObject, RenderQueue & renderQueue,
	Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix)
{
	if (!isVisible) {
		return;
	}

	auto command = MakeUnique<SkinnedMeshRenderCommand>();
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
	POMDOG_ASSERT(polygonBatch);
	polygonBatch->Begin(modelViewProjection);

	Color boneColor {160, 160, 160, 255};

	POMDOG_ASSERT(skeletonTransform);
	auto & globalPose = skeletonTransform->GlobalPose;

	for (auto & joint: *skeleton)
	{
		auto & matrix = globalPose[*joint.Index];

		if (skeleton->Root().Index != joint.Index)
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
		skinningEffect->Parameters("Constants")->SetValue(Matrix4x4::Transpose(modelViewProjection));

		struct MatrixPalette {
			std::array<Vector4, 64> matrixPalette1;
			std::array<Vector4, 64> matrixPalette2;
		};
		MatrixPalette matrixPalette;
		
		std::array<Matrix3x2, 64> matrices;
		
		POMDOG_ASSERT(skeletonTransform);
		auto & globalPose = skeletonTransform->GlobalPose;

		for (auto & joint: *skeleton)
		{
			POMDOG_ASSERT(joint.Index);
			POMDOG_ASSERT(*joint.Index < matrices.size());
			POMDOG_ASSERT(*joint.Index < globalPose.size());
			matrices[*joint.Index] = joint.InverseBindPose * globalPose[*joint.Index];
		}
		
		auto const minMatrixCount = std::min(matrices.size(), globalPose.size());

		for (std::size_t i = 0; i < minMatrixCount; ++i) {
			matrixPalette.matrixPalette1[i].X = matrices[i](0, 0);
			matrixPalette.matrixPalette1[i].Y = matrices[i](0, 1);
			matrixPalette.matrixPalette1[i].Z = matrices[i](1, 0);
			matrixPalette.matrixPalette1[i].W = matrices[i](1, 1);
			matrixPalette.matrixPalette2[i].X = matrices[i](2, 0);
			matrixPalette.matrixPalette2[i].Y = matrices[i](2, 1);
		}

		skinningEffect->Parameters("SkinningConstants")->SetValue(matrixPalette);
	}
	
	{
		graphicsContext->SetTexture(0, texture);
		graphicsContext->SetInputLayout(inputLayout);
		graphicsContext->SetVertexBuffer(mesh->VertexBuffer);
		skinningEffect->Apply();
		graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList,
			mesh->IndexBuffer, mesh->IndexBuffer->IndexCount());
	}
	
//	{
//		RasterizerDescription rasterizerDesc;
//		rasterizerDesc.FillMode = FillMode::WireFrame;
//		auto rasterizerState = std::make_shared<RasterizerState>(gameHost->GraphicsDevice(), rasterizerDesc);
//		
//		graphicsContext->SetRasterizerState(rasterizerState);
//		
//		graphicsContext->SetTexture(0, dummyTexture);
//		graphicsContext->SetInputLayout(inputLayout);
//		graphicsContext->SetVertexBuffer(mesh.VertexBuffer);
//		maidSkinningEffect->Apply();
//		graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList,
//									 mesh.IndexBuffer, mesh.IndexBuffer->IndexCount());
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
