//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_POLYGONBATCH_4158F6A1_3A12_49D9_8B07_DF3FE69BC2AB_HPP
#define POMDOG_POLYGONBATCH_4158F6A1_3A12_49D9_8B07_DF3FE69BC2AB_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class PolygonBatch {
public:
	PolygonBatch(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
	~PolygonBatch();

	void Begin(Matrix4x4 const& transformMatrix);

	//void DrawArc(Vector2 const& position, float radius, Radian<float> const& startAngle, Radian<float> const& arcAngle, Color const& color);
	
	void DrawCircle(Vector2 const& position, float radius, Color const& color, std::size_t segments);

	//void DrawEllipse();

	//void DrawLine(Vector2 const& start, Vector2 const& end, Color const& color);

	//void DrawLine(Vector2 const& start, Vector2 const& end, Color const& startColor, Color const& endColor);

	//void DrawPolyline(std::vector<Vector2> const& points, float thickness, Color const& color);

	void DrawRectangle(Rectangle const& sourceRect, Color const& color);
	
	void DrawRectangle(Rectangle const& sourceRect,
		Color const& color1, Color const& color2, Color const& color3, Color const& color4);

	void DrawRectangle(Matrix3x2 const& matrix, Rectangle const& sourceRect, Color const& color);

	///@note
	/// Y
	/// ^   color4      color3
	/// |    +----------+
	/// |    |          |
	/// |    +----------+
	/// |   color1      color2
	/// |
	/// +-----------------> X
	///
	void DrawRectangle(Matrix3x2 const& matrix, Rectangle const& sourceRect,
		Color const& color1, Color const& color2, Color const& color3, Color const& color4);

	void DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3, Color const& color);

	void DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
		Color const& color1, Color const& color2, Color const& color3);

	//void DrawPolygon(std::vector<Vertex> const& vertices, Color const& color);

	void End();
	
	std::uint32_t DrawCallCount() const;
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_POLYGONBATCH_4158F6A1_3A12_49D9_8B07_DF3FE69BC2AB_HPP)
