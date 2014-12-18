//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Scene.hpp"
#include <vector>
#include <utility>
#include "Transform3D.hpp"
#include "Camera.hpp"
#include "RenderNode.hpp"
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Quaternion.hpp>

namespace Pomdog {
namespace {

namespace CameraHelper
{
	///@~Japanese
	/// @brief カメラの向きを回転します。
	/// @return カメラの向き
	Vector3 Rotate(Vector3 const& axis, Radian<float> const& angle);

	///@~Japanese
	/// @brief カメラの向きを回転します。
	/// @return カメラの向き
	Vector3 Rotate(Quaternion const& orientation);

	///@~Japanese
	/// @brief 現在のカメラの位置から指定された注視点を向くようにカメラの向きを計算します。
	/// @param position カメラの位置
	/// @param target 注視点の位置
	/// @return カメラの向き
	Vector3 LookAt(Vector3 const& position, Vector3 const& target);
	
	///@~Japanese
	/// @brief ビュー行列を作成します。
	Matrix4x4 CreateViewMatrix(Vector3 const& position, Vector3 const& rotation);
}
//-----------------------------------------------------------------------
Vector3 CameraHelper::Rotate(Vector3 const& axis, Radian<float> const& angle)
{
	auto orientation = Quaternion::CreateFromAxisAngle(axis, angle);
	auto rotationMatrix = Matrix4x4::CreateFromQuaternion(std::move(orientation));
	return Vector3::Transform(Vector3::UnitZ, std::move(rotationMatrix));
}
//-----------------------------------------------------------------------
Vector3 CameraHelper::Rotate(Quaternion const& orientation)
{
	auto rotationMatrix = Matrix4x4::CreateFromQuaternion(orientation);
	return Vector3::Transform(Vector3::UnitZ, std::move(rotationMatrix));
}
//-----------------------------------------------------------------------
Vector3 CameraHelper::LookAt(Vector3 const& position, Vector3 const& target)
{
	return (target - position);
}
//-----------------------------------------------------------------------
Matrix4x4 CameraHelper::CreateViewMatrix(Vector3 const& position, Vector3 const& rotation)
{
	auto quaternion = Quaternion::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z);///@todo badcode
	auto lookDirection = Rotate(quaternion);
	return Matrix4x4::CreateLookAtLH(position, position + lookDirection, Vector3::UnitY);
}

}// unnamed namespace
//-----------------------------------------------------------------------
class Scene::Impl
{
public:
	explicit Impl(std::shared_ptr<GraphicsContext> const& graphicsContext);

public:
	std::vector<std::shared_ptr<RenderNode>> children;
	
	Camera camera;
	Transform3D cameraTransform;

	std::shared_ptr<GraphicsContext> graphicsContext;
};
//-----------------------------------------------------------------------
Scene::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContextIn)
	: graphicsContext(graphicsContextIn)
{
	POMDOG_ASSERT(graphicsContext);
}
//-----------------------------------------------------------------------
Scene::Scene(std::shared_ptr<GraphicsContext> const& graphicsContextIn)
	: impl(new Impl(graphicsContextIn))
{}
//-----------------------------------------------------------------------
Scene::~Scene() = default;
//-----------------------------------------------------------------------
void Scene::AddChild(std::shared_ptr<RenderNode> const& node)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(node);
	impl->children.push_back(node);
}
//-----------------------------------------------------------------------
void Scene::AddChild(std::shared_ptr<RenderNode> && node)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(node);
	impl->children.push_back(std::move(node));
}
//-----------------------------------------------------------------------
void Scene::RemoveChild(std::shared_ptr<RenderNode> const& node)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(node);
	auto & children = impl->children;
	children.erase(std::remove(std::begin(children), std::end(children), node),
		std::end(children));
}
//-----------------------------------------------------------------------
void Scene::Render(/*double deltaMilliseconds*/)
{
	POMDOG_ASSERT(impl);

	auto & camera = impl->camera;

	RenderingContext context;
	context.graphicsContext = impl->graphicsContext;
	context.ViewMatrix = CameraHelper::CreateViewMatrix(impl->cameraTransform.Position, impl->cameraTransform.Rotation);
	context.ProjectionMatrix = camera.CreateProjectionMatrix();
	context.CameraPosition = impl->cameraTransform.Position;

	for (auto & node: impl->children)
	{
		node->Draw(context);
	}
}

}// namespace Pomdog
