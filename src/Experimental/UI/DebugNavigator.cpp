// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/DebugNavigator.hpp"
#include "Pomdog/Experimental/UI/DrawingContext.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <cmath>

namespace Pomdog::UI {
namespace {

constexpr float minFramerate = 10.0f;
constexpr float maxFramerate = 60.0f;
constexpr std::size_t maxHistories = 40;

} // namespace

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

void DebugNavigator::Draw(DrawingContext& drawingContext)
{
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

    auto globalTransform = GetTransform() * drawingContext.Top();
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    constexpr float maxGraphHeight = 26.0f;
    constexpr float graphInnerMarginLeft = 1.0f;
    constexpr float graphMargin = 2.0f;

    const auto graghWidth = (static_cast<float>(GetWidth()) - (graphMargin * 2.0f)) / maxHistories;
    const auto barWidth = graghWidth - graphInnerMarginLeft;

    const Color chartBackgroundColor = {109, 109, 109, 255};
    const Color chartBarColor = {112, 202, 255, 255};

    primitiveBatch->DrawRectangle(
        globalTransform,
        Vector2::Zero,
        static_cast<float>(GetWidth()),
        maxGraphHeight,
        chartBackgroundColor);

    auto startPosition = graphMargin + (graghWidth * static_cast<float>(maxHistories - frameRates.size()));
    auto graphX = startPosition;
    for (auto& frameRate : frameRates) {
        auto amount = ((frameRate - minFramerate) / (maxFramerate - minFramerate));
        auto barHeight = MathHelper::Clamp((maxGraphHeight - (graphMargin * 2)) * amount, 1.0f, maxGraphHeight);

        primitiveBatch->DrawRectangle(
            globalTransform,
            Vector2{graphX, graphMargin},
            barWidth,
            barHeight,
            chartBarColor);
        graphX += graghWidth;
    }
    primitiveBatch->Flush();

    auto textPosition = Vector2{globalTransform(2, 0), globalTransform(2, 1)} + Vector2{0.0f, maxGraphHeight + 5.0f};

    auto spriteBatch = drawingContext.GetSpriteBatch();
    auto spriteFont = drawingContext.GetFont(FontWeight::Bold, FontSize::Medium);

    spriteFont->Draw(*spriteBatch, frameRateString, textPosition, Color{198, 198, 198, 255});
}

} // namespace Pomdog::UI
