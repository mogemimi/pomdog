//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkinnedMeshRenderable.hpp"
#include "../Rendering/Renderer.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonHelper.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkinnedMesh.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"

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
SkinnedMeshRenderable::SkinnedMeshRenderable(GraphicsDevice & graphicsDevice,
	std::shared_ptr<Skeleton> const& skeletonIn,
	std::shared_ptr<SkeletonTransform> const& skeletonTransformIn,
	std::shared_ptr<SkinnedMesh> const& meshIn, std::shared_ptr<Texture2D> const& textureIn)
	: command(graphicsDevice)
	, skeleton(skeletonIn)
	, skeletonTransform(skeletonTransformIn)
{
	POMDOG_ASSERT(skeleton);
	POMDOG_ASSERT(skeletonTransform);

	command.mesh = meshIn;
	command.skinnedEffect.SetTexture(textureIn);
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderable::Visit(GameObject & gameObject, Renderer & renderer,
	Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix)
{
	if (!IsVisible) {
		return;
	}
	
	command.drawOrder = DrawOrder;

	POMDOG_ASSERT(skeleton);
	POMDOG_ASSERT(skeletonTransform);
	
	command.SetMatrixPalette(*skeleton, *skeletonTransform);
	
	auto worldViewProjection = viewMatrix * projectionMatrix;
	
	if (auto transform = gameObject.Component<Transform2D>())
	{
		worldViewProjection = CreateTransformMatrix4x4(*transform) * worldViewProjection;
	}
	
	command.skinnedEffect.SetWorldViewProjection(worldViewProjection);
	
	renderer.PushCommand(command);
}
//-----------------------------------------------------------------------
void SkinnedMeshRenderable::DrawSkeleton(std::unique_ptr<PolygonBatch> const& polygonBatch,
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
}// namespace Pomdog
