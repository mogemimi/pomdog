//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "DebugNavigator.hpp"
#include "DrawingContext.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
DebugNavigator::DebugNavigator(std::shared_ptr<GameClock> const& clockIn)
	: Control(Matrix3x2::Identity, 150, 40)
	, clock(clockIn)
	, duration(DurationSeconds(0))
	, frameRateString("-- fps")
{}
//-----------------------------------------------------------------------
void DebugNavigator::Draw(DrawingContext & drawingContext)
{
	constexpr float minFramerate = 10.0f;
	constexpr float maxFramerate = 60.0f;
	constexpr std::uint16_t maxHistories = 20;
	
	{
		if (clock->TotalGameTime() - duration > DurationSeconds(0.2))
		{
			auto frameRate = clock->FrameRate();
			frameRateString = StringFormat("%4.2f fps", frameRate);
			frameRates.push_back(MathHelper::Clamp(frameRate, minFramerate, maxFramerate));
			
			if (frameRates.size() > maxHistories)
			{
				frameRates.pop_front();
			}
			duration = clock->TotalGameTime();
		}
	}
	
	auto transform = Transform() * drawingContext.Top();
	{
		auto graphTransform = Matrix3x2::CreateTranslation(Vector2{0, 16}) * transform;
	
		constexpr std::uint16_t maxGraphHeight = 26;
		constexpr float graphMarginLeft = 1.0f;

		auto graghWidth = (static_cast<float>(Width()) / maxHistories);

		std::int32_t graphX = 0;
		for (auto & frameRate: frameRates)
		{
			auto amount = ((frameRate - minFramerate) / (maxFramerate - minFramerate));
			auto graphHeight = MathHelper::Clamp<std::uint16_t>(maxGraphHeight * amount, 1, maxGraphHeight);

			drawingContext.DrawRectangle(graphTransform, Color::CornflowerBlue,
				Rectangle(graphX, maxGraphHeight - graphHeight, graghWidth - graphMarginLeft, graphHeight));
			graphX += graghWidth;
		}
	}

	drawingContext.DrawString(transform * Matrix3x2::CreateTranslation(Vector2(5, -2.5f)),
		Color::White, FontWeight::Bold, FontSize::Medium, frameRateString);
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
