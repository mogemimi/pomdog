//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_LINERENDERER_F1900C08_2DB6_40A0_B166_3873798DF8D8_HPP
#define POMDOG_LINERENDERER_F1900C08_2DB6_40A0_B166_3873798DF8D8_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <vector>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class LineRenderer {
public:
	explicit LineRenderer(std::shared_ptr<GameHost> const& gameHost);

	void Begin(Matrix4x4 const& transformMatrix);
	
	void Draw(Vector3 const& point1, Vector3 const& point2, Color const& color);
	
	void End();
	
private:
	static constexpr std::uint16_t MaxLines = 1028;

	struct LinePoint
	{
		Vector3 Position;
		Vector4 Color;
	};

	std::vector<LinePoint> points;
	
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace Pomdog

#endif // !defined(POMDOG_LINERENDERER_F1900C08_2DB6_40A0_B166_3873798DF8D8_HPP)
