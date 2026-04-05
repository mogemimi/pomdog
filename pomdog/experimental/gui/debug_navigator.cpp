// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/debug_navigator.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
namespace {

constexpr float minFramerate = 10.0f;
constexpr float maxFramerate = 60.0f;
constexpr std::size_t maxHistories = 40;

} // namespace

DebugNavigator::DebugNavigator(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    const std::shared_ptr<GameClock>& clockIn)
    : Widget(dispatcher)
    , clock_(clockIn)
    , duration_(Duration::zero())
    , frameRateString_("-- fps")
{
    setSize(150, 40);
}

void DebugNavigator::draw(DrawingContext& drawingContext)
{
    {
        if (clock_->getTotalGameTime() - duration_ > Duration(0.2)) {
            auto frameRate = clock_->getFrameRate();
            frameRateString_ = pomdog::format("{:4.2f} fps", frameRate);
            frameRates_.push_back(std::round(math::clamp(frameRate, minFramerate, maxFramerate)));

            if (frameRates_.size() > maxHistories) {
                frameRates_.pop_front();
            }
            duration_ = clock_->getTotalGameTime();
        }
    }

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    constexpr float maxGraphHeight = 26.0f;
    constexpr float graphInnerMarginLeft = 1.0f;
    constexpr float graphMargin = 2.0f;

    const auto graghWidth = (static_cast<float>(getWidth()) - (graphMargin * 2.0f)) / maxHistories;
    const auto barWidth = graghWidth - graphInnerMarginLeft;

    const Color chartBackgroundColor = {109, 109, 109, 255};
    const Color chartBarColor = {112, 202, 255, 255};

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x, globalPos.y, getWidth(), static_cast<int>(maxGraphHeight)},
        chartBackgroundColor);

    auto startPosition = graphMargin + (graghWidth * static_cast<float>(maxHistories - frameRates_.size()));
    auto graphX = startPosition;
    for (auto& frameRate : frameRates_) {
        auto amount = ((frameRate - minFramerate) / (maxFramerate - minFramerate));
        auto barHeight = math::clamp((maxGraphHeight - (graphMargin * 2)) * amount, 1.0f, maxGraphHeight);

        primitiveBatch->drawRectangle(
            Matrix3x2::createIdentity(),
            Vector2{graphX, graphMargin} + math::toVector2(globalPos),
            barWidth,
            barHeight,
            chartBarColor);
        graphX += graghWidth;
    }
    primitiveBatch->flush();

    auto textPosition = math::toVector2(globalPos) + Vector2{0.0f, maxGraphHeight + 5.0f};

    auto spriteBatch = drawingContext.getSpriteBatch();
    auto spriteFont = drawingContext.getFont(FontWeight::Bold, FontSize::Medium);

    spriteFont->draw(*spriteBatch, frameRateString_, textPosition, Color{198, 198, 198, 255});
}

} // namespace pomdog::gui
