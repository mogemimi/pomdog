//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SCENENODE_5C516A46_97D1_455B_851C_45DCAB4E5605_HPP
#define POMDOG_SCENENODE_5C516A46_97D1_455B_851C_45DCAB4E5605_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SceneNode: public HierarchyNode {
public:
	SceneNode();

	explicit SceneNode(std::shared_ptr<GameObject> const& object);

	virtual ~SceneNode() = default;
	
	virtual void Update(DurationSeconds const& frameDuration) = 0;

	virtual void Draw(/* DrawingContext const& drawingContext*/) = 0;

	void UpdateTransformMatrix();
	
	///@note transform matrix in local space
	Matrix3x2 const& TransformMatrix() const;

	void SetPosition(Vector2 const& position);
	
	Vector2 GetPosition() const;
	
	void SetScale(Vector2 const& scale);
	
	Vector2 GetScale() const;
	
	void SetRotation(Radian<float> const& rotation);
	
	Radian<float> GetRotation() const;
	
	//virtual void SetVisible(bool isVisible) = 0;
	
	//virtual bool IsVisible() const = 0;

	std::int32_t ZOrder() const;
	
private:
	Transform2D transform;
	Matrix3x2 transformMatrix;
	std::int32_t zOrder;
	bool isTransformDirty;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCENENODE_5C516A46_97D1_455B_851C_45DCAB4E5605_HPP)
