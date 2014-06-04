//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GRADIENTPLANE_A123BDFD_A1B3_4203_BADC_F8622DBBD641_HPP
#define POMDOG_GRADIENTPLANE_A123BDFD_A1B3_4203_BADC_F8622DBBD641_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <vector>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace SceneEditor {

class GradientPlane {
public:
	explicit GradientPlane(std::shared_ptr<GameHost> const& gameHost);

	void Draw();
	
private:
	struct Point {
		Vector3 Position;
		Vector4 Color;
	};
	
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace SceneEditor
}// namespace Pomdog

#endif // !defined(POMDOG_GRADIENTPLANE_A123BDFD_A1B3_4203_BADC_F8622DBBD641_HPP)
