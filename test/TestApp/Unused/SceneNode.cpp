//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SceneNode.hpp"

namespace Pomdog {
namespace {

Matrix3x2 ToMatrix(Transform2D const& transform)
{
	auto matrix = Matrix3x2::CreateScale(transform.Scale)
		* Matrix3x2::CreateRotation(transform.Rotation);
	matrix(2, 0) = transform.Position.X;
	matrix(2, 1) = transform.Position.Y;
	return std::move(matrix);
}

}// unnamed namespace
//-----------------------------------------------------------------------
SceneNode::SceneNode()
	: HierarchyNode()
	, transform()
	, transformMatrix(Matrix3x2::Identity)
	, zOrder(0)
	, isTransformDirty(true)
{}
//-----------------------------------------------------------------------
SceneNode::SceneNode(std::shared_ptr<GameObject> const& objectIn)
	: HierarchyNode(objectIn)
	, transform()
	, transformMatrix(Matrix3x2::Identity)
	, isTransformDirty(true)
{}
//-----------------------------------------------------------------------
void SceneNode::UpdateTransformMatrix()
{
	if (!isTransformDirty) {
		return;
	}
	
	transformMatrix = ToMatrix(transform);
	isTransformDirty = false;
}
//-----------------------------------------------------------------------
Matrix3x2 const& SceneNode::TransformMatrix() const
{
	POMDOG_ASSERT(isTransformDirty);
	return transformMatrix;
}
//-----------------------------------------------------------------------
void SceneNode::SetPosition(Vector2 const& position)
{
	this->transform.Position = position;
	isTransformDirty = true;
}
//-----------------------------------------------------------------------
Vector2 SceneNode::GetPosition() const
{
	return transform.Position;
}
//-----------------------------------------------------------------------
void SceneNode::SetScale(Vector2 const& scale)
{
	this->transform.Scale = scale;
	isTransformDirty = true;
}
//-----------------------------------------------------------------------
Vector2 SceneNode::GetScale() const
{
	return transform.Scale;
}
//-----------------------------------------------------------------------
void SceneNode::SetRotation(Radian<float> const& rotation)
{
	this->transform.Rotation = rotation;
	isTransformDirty = true;
}
//-----------------------------------------------------------------------
Radian<float> SceneNode::GetRotation() const
{
	return transform.Rotation;
}
//-----------------------------------------------------------------------
std::int32_t SceneNode::ZOrder() const
{
	return zOrder;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
