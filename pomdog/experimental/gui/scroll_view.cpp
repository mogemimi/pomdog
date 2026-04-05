// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/scroll_view.h"
#include "pomdog/basic/platform.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/experimental/gui/vertical_scroll_bar.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

ScrollView::ScrollView(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , margin_{0, 0, 0, 0}
    , backgroundColor_(Color::createTransparentBlack())
    , horizontalAlignment_(HorizontalAlignment::Stretch)
    , needToUpdateLayout_(true)
{
    setSize(widthIn, heightIn);
    setWheelFocusEnabled(true);

    scrollBar_ = std::make_shared<VerticalScrollBar>(dispatcher, 0.0, 100.0);
    scrollBar_->setMinimum(0.0);
    scrollBar_->setMaximum(100.0);
    scrollBar_->setPageStep(30.0);
    scrollBar_->setValue(0.0);
}

void ScrollView::setMargin(const Thickness& marginIn)
{
    margin_ = marginIn;
    needToUpdateLayout_ = true;
}

void ScrollView::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    if (scrollBar_ != nullptr) {
        scrollBar_->markParentTransformDirty();
    }
    if (child_ != nullptr) {
        child_->markParentTransformDirty();
    }
}

void ScrollView::setBackgroundColor(const Color& color) noexcept
{
    backgroundColor_ = color;
}

void ScrollView::markParentTransformDirty()
{
    Widget::markParentTransformDirty();

    if (scrollBar_ != nullptr) {
        scrollBar_->markParentTransformDirty();
    }
    if (child_ != nullptr) {
        child_->markParentTransformDirty();
    }
}

void ScrollView::markContentLayoutDirty()
{
    needToUpdateLayout_ = true;
}

bool ScrollView::getSizeToFitContent() const noexcept
{
    return false;
}

void ScrollView::scrollToTop()
{
    if (child_ == nullptr) {
        return;
    }

    child_->doLayout();

    if (child_->getHeight() > getHeight()) {
        scrollBar_->setValue(scrollBar_->getMaximum());
        needToUpdateLayout_ = true;
    }
}

void ScrollView::scrollToEnd()
{
    if (child_ == nullptr) {
        return;
    }

    child_->doLayout();

    if (child_->getHeight() > getHeight()) {
        scrollBar_->setValue(scrollBar_->getMinimum());
        needToUpdateLayout_ = true;
    }
}

void ScrollView::scrollTo(const std::shared_ptr<Widget>& widget)
{
    if (child_ == nullptr) {
        return;
    }
    if (widget == nullptr) {
        return;
    }

    child_->doLayout();

    if (child_->getHeight() > getHeight()) {
        const auto w = widget->getGlobalPosition();
        const auto s = scrollBar_->getGlobalPosition();
        const auto h = scrollBar_->getHeight();
        const auto v = w.y - (s.y + h);
        scrollBar_->setValue(std::clamp(static_cast<double>(v), scrollBar_->getMinimum(), scrollBar_->getMaximum()));
        needToUpdateLayout_ = true;
    }
}

void ScrollView::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    horizontalAlignment_ = horizontalAlignmentIn;
}

HorizontalAlignment ScrollView::getHorizontalAlignment() const noexcept
{
    return horizontalAlignment_;
}

void ScrollView::onEnter()
{
    POMDOG_ASSERT(scrollBar_ != nullptr);
    POMDOG_ASSERT(shared_from_this());
    scrollBar_->setParent(shared_from_this());
    scrollBar_->markParentTransformDirty();
    scrollBar_->onEnter();
}

void ScrollView::onPointerWheelChanged(const PointerPoint& pointerPoint)
{
#if defined(POMDOG_PLATFORM_WIN32)
    // FIXME: Set to appropriate wheel scroll speed for each platform.
    constexpr int divisor = 400;
#else
    // NOTE: The answer to life, universe and everything.
    constexpr int divisor = 42;
#endif
    const auto wheelSpeed = scrollBar_->getPageStep() / divisor;

    const auto v = scrollBar_->getValue() + wheelSpeed * pointerPoint.MouseWheelDelta;
    scrollBar_->setValue(std::clamp(v, scrollBar_->getMinimum(), scrollBar_->getMaximum()));

    needToUpdateLayout_ = true;
}

void ScrollView::setWidget(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget != nullptr);
    POMDOG_ASSERT(!widget->getParent());

    child_ = widget;
    child_->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    child_->setParent(shared_from_this());
    child_->onEnter();

    if (child_->getHeight() <= getHeight()) {
        scrollBar_->setEnabled(false);
        scrollBar_->setVisible(false);
    }
    else {
        scrollBar_->setEnabled(true);
        scrollBar_->setVisible(true);

        scrollBar_->setMinimum(0.0);
        scrollBar_->setMaximum(static_cast<double>(child_->getHeight() - getHeight()));
        scrollBar_->setPageStep(static_cast<double>(getHeight()));
        scrollBar_->setValue(scrollBar_->getMaximum());
    }

    needToUpdateLayout_ = true;
}

std::shared_ptr<Widget> ScrollView::getChildAt(const Point2D& position)
{
    if (scrollBar_ != nullptr) {
        auto bounds = scrollBar_->getBounds();
        if (bounds.contains(position)) {
            return scrollBar_;
        }
    }

    auto clipBounds = Rect2D{0, 0, getWidth(), getHeight()};
    if (!clipBounds.contains(position)) {
        return nullptr;
    }

    if (child_ != nullptr) {
        auto bounds = child_->getBounds();
        if (bounds.contains(position)) {
            return child_;
        }
    }
    return nullptr;
}

void ScrollView::updateAnimation(const Duration& frameDuration)
{
    if (scrollBar_ != nullptr) {
        scrollBar_->updateAnimation(frameDuration);
    }
    if (child_ != nullptr) {
        child_->updateAnimation(frameDuration);
    }
}

void ScrollView::updateLayout()
{
    if (scrollBar_ != nullptr) {
        scrollBar_->doLayout();
    }
    if (child_ != nullptr) {
        child_->doLayout();
    }

    if (!needToUpdateLayout_) {
        return;
    }
    needToUpdateLayout_ = false;

    if (child_ == nullptr) {
        // NOTE: Resizing this panel
        setSize(getWidth(), 0);

        if (auto parent = getParent(); parent != nullptr) {
            parent->markContentLayoutDirty();
        }
        return;
    }

    POMDOG_ASSERT(child_ != nullptr);

    if (auto parent = getParent(); parent != nullptr) {
        parent->markContentLayoutDirty();
    }

    scrollBar_->setSize(scrollBar_->getWidth(), getHeight());
    scrollBar_->setPosition(Point2D{getWidth() - scrollBar_->getWidth(), 0});
    scrollBar_->setMinimum(0.0);
    scrollBar_->setPageStep(static_cast<double>(getHeight()));
    scrollBar_->markContentLayoutDirty();

    // NOTE: update layout for child_
    if (child_ != nullptr) {
        POMDOG_ASSERT(scrollBar_ != nullptr);

        const auto scrollBarVisible = child_->getHeight() > getHeight();

        if (scrollBarVisible) {
            child_->setSize(getWidth() - scrollBar_->getWidth(), child_->getHeight());
            child_->setPosition(Point2D{0, static_cast<int>(-scrollBar_->getValue())});
        }
        else {
            child_->setSize(getWidth(), child_->getHeight());
            child_->setPosition(Point2D{0, getHeight() - child_->getHeight()});
        }

        child_->markContentLayoutDirty();
        child_->doLayout();

        if (!scrollBarVisible) {
            scrollBar_->setEnabled(false);
            scrollBar_->setVisible(false);
            setWheelFocusEnabled(false);
        }
        else {
            scrollBar_->setEnabled(true);
            scrollBar_->setVisible(true);
            setWheelFocusEnabled(true);

            const auto oldMaxValue = scrollBar_->getMaximum();
            scrollBar_->setMaximum(static_cast<double>(child_->getHeight() - getHeight()));
            scrollBar_->setPageStep(static_cast<double>(getHeight()));

            if (oldMaxValue < scrollBar_->getMaximum()) {
                scrollBar_->setValue(scrollBar_->getMaximum() - (oldMaxValue - scrollBar_->getValue()));
            }
            else if (oldMaxValue > scrollBar_->getMaximum()) {
                scrollBar_->setValue(std::min(scrollBar_->getValue(), scrollBar_->getMaximum()));
            }
        }
    }
    else {
        scrollBar_->setMaximum(static_cast<double>(getHeight()));
    }

    // NOTE: update layout for scroll bar
    scrollBar_->doLayout();
}

void ScrollView::doLayout()
{
    updateLayout();
}

void ScrollView::draw(DrawingContext& drawingContext)
{
    updateLayout();

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    if (backgroundColor_.a > 0) {
        auto primitiveBatch = drawingContext.getPrimitiveBatch();

        primitiveBatch->drawRectangle(
            Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
            backgroundColor_);

        primitiveBatch->flush();
    }

    auto innerBoundPos = math::toVector2(globalPos);

    // NOTE: Mask scissor
    drawingContext.pushScissorRect(Rect2D{
        static_cast<int>(innerBoundPos.x),
        static_cast<int>(innerBoundPos.y),
        getWidth(),
        getHeight()});

    drawingContext.pushTransform(globalPos);

    if (child_ != nullptr) {
        child_->draw(drawingContext);
    }

    drawingContext.popScissorRect();

    if ((scrollBar_ != nullptr) && scrollBar_->isVisible()) {
        scrollBar_->draw(drawingContext);
    }

    drawingContext.popTransform();
}

} // namespace pomdog::gui
