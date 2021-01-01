// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/DebugNavigator.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Math/Math.hpp"
#include "Pomdog/Utility/StringHelper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {
namespace {

constexpr float minFramerate = 10.0f;
constexpr float maxFramerate = 60.0f;
constexpr std::size_t maxHistories = 40;

} // namespace

DebugNavigator::DebugNavigator(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    const std::shared_ptr<GameClock>& clockIn)
    : Widget(dispatcher)
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
            frameRateString = StringHelper::Format("%4.2lf fps", static_cast<double>(frameRate));
            frameRates.push_back(std::round(Math::Clamp(frameRate, minFramerate, maxFramerate)));

            if (frameRates.size() > maxHistories) {
                frameRates.pop_front();
            }
            duration = clock->GetTotalGameTime();
        }
    }

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    constexpr float maxGraphHeight = 26.0f;
    constexpr float graphInnerMarginLeft = 1.0f;
    constexpr float graphMargin = 2.0f;

    const auto graghWidth = (static_cast<float>(GetWidth()) - (graphMargin * 2.0f)) / maxHistories;
    const auto barWidth = graghWidth - graphInnerMarginLeft;

    const Color chartBackgroundColor = {109, 109, 109, 255};
    const Color chartBarColor = {112, 202, 255, 255};

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X, globalPos.Y, GetWidth(), static_cast<int>(maxGraphHeight)},
        chartBackgroundColor);

    auto startPosition = graphMargin + (graghWidth * static_cast<float>(maxHistories - frameRates.size()));
    auto graphX = startPosition;
    for (auto& frameRate : frameRates) {
        auto amount = ((frameRate - minFramerate) / (maxFramerate - minFramerate));
        auto barHeight = Math::Clamp((maxGraphHeight - (graphMargin * 2)) * amount, 1.0f, maxGraphHeight);

        primitiveBatch->DrawRectangle(
            Matrix3x2::Identity,
            Vector2{graphX, graphMargin} + Math::ToVector2(globalPos),
            barWidth,
            barHeight,
            chartBarColor);
        graphX += graghWidth;
    }
    primitiveBatch->Flush();

    auto textPosition = Math::ToVector2(globalPos) + Vector2{0.0f, maxGraphHeight + 5.0f};

    auto spriteBatch = drawingContext.GetSpriteBatch();
    auto spriteFont = drawingContext.GetFont(FontWeight::Bold, FontSize::Medium);

    spriteFont->Draw(*spriteBatch, frameRateString, textPosition, Color{198, 198, 198, 255});
}

} // namespace Pomdog::GUI
