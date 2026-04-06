// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

StackPanel::StackPanel(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , padding_{12, 8, 10, 8}
    , barHeight_(16)
    , needToUpdateLayout_(true)
{
    setSize(widthIn, heightIn);

    verticalLayout_ = std::make_shared<VerticalLayout>(dispatcher, 140, 10);
    verticalLayout_->setStackedLayout(true);
    verticalLayout_->setLayoutSpacing(12);
}

void StackPanel::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->markParentTransformDirty();
}

void StackPanel::setPadding(const Thickness& paddingIn)
{
    padding_ = paddingIn;
    needToUpdateLayout_ = true;
}

void StackPanel::markParentTransformDirty()
{
    Widget::markParentTransformDirty();

    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->markParentTransformDirty();
}

void StackPanel::markContentLayoutDirty()
{
    needToUpdateLayout_ = true;
}

bool StackPanel::getSizeToFitContent() const noexcept
{
    return false;
}

void StackPanel::onEnter()
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    verticalLayout_->setParent(shared_from_this());
    verticalLayout_->onEnter();
}

void StackPanel::onPointerPressed(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && (*pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed)) {
        return;
    }

    auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());

    const auto collisionHeight = barHeight_ + padding_.top;
    Rect2D captionBar{
        0,
        getHeight() - collisionHeight,
        getWidth(),
        collisionHeight};

    if (captionBar.contains(pointInView)) {
        startTouchPoint_ = math::toVector2(pointInView);
    }
}

void StackPanel::onPointerMoved(const PointerPoint& pointerPoint)
{
    if (!startTouchPoint_) {
        return;
    }

    auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());
    auto position = math::toVector2(pointInView);

    auto tangent = position - *startTouchPoint_;
    auto distanceSquared = math::lengthSquared(tangent);

    if (distanceSquared >= 1.4143f) {
        setPosition(getPosition() + math::toPoint2D(tangent));

        // NOTE: recalculate position in current coordinate system
        pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());
        position = math::toVector2(pointInView);
        startTouchPoint_ = position;
    }
}

void StackPanel::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!startTouchPoint_) {
        return;
    }

    startTouchPoint_ = std::nullopt;
}

void StackPanel::addChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->addChild(widget);
    needToUpdateLayout_ = true;
}

std::shared_ptr<Widget> StackPanel::getChildAt(const Point2D& position)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    auto bounds = verticalLayout_->getBounds();
    if (bounds.contains(position)) {
        return verticalLayout_;
    }
    return nullptr;
}

void StackPanel::updateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->updateAnimation(frameDuration);
}

void StackPanel::updateLayout()
{
    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->doLayout();

    if (!needToUpdateLayout_) {
        return;
    }

    const auto requiredHeight = padding_.top + barHeight_ + verticalLayout_->getHeight() + padding_.bottom;
    if (requiredHeight != getHeight()) {
        // NOTE: Keeping the original position
        const auto positionOffset = Point2D{0, getHeight() - requiredHeight};
        setPosition(getPosition() + positionOffset);

        // NOTE: Resizing this panel
        setSize(getWidth(), requiredHeight);

        auto parent = getParent();
        if (parent) {
            parent->markContentLayoutDirty();
        }
    }

    // NOTE: update layout for children
    {
        verticalLayout_->setPosition(Point2D{padding_.left, padding_.bottom});

        switch (verticalLayout_->getHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = getWidth() - (padding_.left + padding_.right);
            verticalLayout_->setSize(childWidth, verticalLayout_->getHeight());
            verticalLayout_->markContentLayoutDirty();
            break;
        }
        case HorizontalAlignment::Left:
            break;
        case HorizontalAlignment::Right:
            break;
        }
    }

    needToUpdateLayout_ = false;
}

void StackPanel::doLayout()
{
    updateLayout();
}

void StackPanel::draw(DrawingContext& drawingContext)
{
    updateLayout();
    POMDOG_ASSERT(!needToUpdateLayout_);

    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    const auto w = static_cast<float>(getWidth());
    const auto h = static_cast<float>(getHeight());

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
        colorScheme->PanelBackgroundColor);

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x, globalPos.y + (getHeight() - barHeight_), getWidth(), barHeight_},
        colorScheme->PanelTitleBarColor);

    const auto pos = math::toVector2(globalPos);
    primitiveBatch->drawLine(pos + Vector2{0.0f, 0.0f}, pos + Vector2{w, 0.0f}, colorScheme->PanelOutlineBorderColor, 1.0f);
    primitiveBatch->drawLine(pos + Vector2{0.0f, 0.0f}, pos + Vector2{0.0f, h}, colorScheme->PanelOutlineBorderColor, 1.0f);
    primitiveBatch->drawLine(pos + Vector2{0.0f, h}, pos + Vector2{w, h}, colorScheme->PanelOutlineBorderColor, 1.0f);
    primitiveBatch->drawLine(pos + Vector2{w, 0.0f}, pos + Vector2{w, h}, colorScheme->PanelOutlineBorderColor, 1.0f);
    drawingContext.flushPrimitiveBatch();

    drawingContext.pushTransform(globalPos);

    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->draw(drawingContext);

    drawingContext.popTransform();
}

} // namespace pomdog::gui
