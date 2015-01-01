//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_RENDERNODE_9E4CE0EA_E4CE_46FB_940D_73B535E9C035_HPP
#define POMDOG_RENDERNODE_9E4CE0EA_E4CE_46FB_940D_73B535E9C035_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <memory>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Vector3.hpp>

namespace Pomdog {

class RenderingContext
{
public:
	std::shared_ptr<GraphicsContext> graphicsContext;
	Matrix4x4 ViewMatrix;
	Matrix4x4 ProjectionMatrix;
	Vector3 CameraPosition;
};

class RenderNode
{
public:
	virtual ~RenderNode() = default;

	virtual void Draw(RenderingContext const& context) = 0;

	///@~Japanese
	/// @brief ノードが表示されるかどうかを取得します。
	/// @return 表示する場合は true を、非表示の場合は false を返却します。
	virtual bool IsVisible() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERNODE_9E4CE0EA_E4CE_46FB_940D_73B535E9C035_HPP)
