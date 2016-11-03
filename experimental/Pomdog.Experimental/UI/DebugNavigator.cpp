// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/DebugNavigator.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"

namespace Pomdog {
namespace UI {

DebugNavigator::DebugNavigator(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    const std::shared_ptr<GameClock>& clockIn)
    : UIElement(dispatcher)
    , clock(clockIn)
    , duration(Duration::zero())
    , frameRateString("-- fps")
{
    SetSize(150, 40);
}

void DebugNavigator::Draw(DrawingContext & drawingContext)
{
    constexpr float minFramerate = 10.0f;
    constexpr float maxFramerate = 60.0f;
    constexpr std::size_t maxHistories = 20;

    {
        if (clock->GetTotalGameTime() - duration > Duration(0.2)) {
            auto frameRate = clock->GetFrameRate();
            frameRateString = StringHelper::Format("%4.2f fps", frameRate);
            frameRates.push_back(std::round(MathHelper::Clamp(frameRate, minFramerate, maxFramerate)));

            if (frameRates.size() > maxHistories) {
                frameRates.pop_front();
            }
            duration = clock->GetTotalGameTime();
        }
    }

    auto transform = GetTransform() * drawingContext.Top();
    {
        auto graphTransform = Matrix3x2::CreateTranslation(Vector2{0, 16}) * transform;

        constexpr std::uint16_t maxGraphHeight = 26;
        constexpr float graphMarginLeft = 1.0f;

        auto graghWidth = (static_cast<float>(GetWidth()) / maxHistories);

        std::int32_t startPosition = graghWidth * (maxHistories - frameRates.size());
        std::int32_t graphX = startPosition;
        for (auto & frameRate : frameRates) {
            auto amount = ((frameRate - minFramerate) / (maxFramerate - minFramerate));
            auto graphHeight = MathHelper::Clamp<std::uint16_t>(maxGraphHeight * amount, 1, maxGraphHeight);

            drawingContext.DrawRectangle(graphTransform, Color::CornflowerBlue,
                Rectangle(graphX, maxGraphHeight - graphHeight, graghWidth - graphMarginLeft, graphHeight));
            graphX += graghWidth;
        }
    }

    drawingContext.DrawString(transform * Matrix3x2::CreateTranslation({0.5f, -2.5f}),
        Color::White, FontWeight::Bold, FontSize::Medium, frameRateString);
}

} // namespace UI
} // namespace Pomdog
