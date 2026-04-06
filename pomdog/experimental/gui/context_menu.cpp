// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/context_menu.h"
#include "pomdog/experimental/gui/context_menu_item.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/scroll_view.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

ContextMenu::ContextMenu(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , padding_{8, 1, 8, 1}
    , needToUpdateLayout_(true)
{
    setSize(widthIn, heightIn);

    verticalLayout_ = std::make_shared<VerticalLayout>(dispatcher, 140, 10);
    verticalLayout_->setStackedLayout(true);
    verticalLayout_->setLayoutSpacing(0);

    scrollView_ = std::make_shared<gui::ScrollView>(dispatcher, 140, 100);
    scrollView_->setWidget(verticalLayout_);

    setHierarchySortOrder(HierarchySortOrder::Front);
}

void ContextMenu::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    POMDOG_ASSERT(scrollView_ != nullptr);
    scrollView_->markParentTransformDirty();
}

void ContextMenu::setPadding(const Thickness& paddingIn)
{
    padding_ = paddingIn;
    needToUpdateLayout_ = true;
}

void ContextMenu::markParentTransformDirty()
{
    Widget::markParentTransformDirty();

    POMDOG_ASSERT(scrollView_);
    scrollView_->markParentTransformDirty();
}

void ContextMenu::markContentLayoutDirty()
{
    needToUpdateLayout_ = true;
}

bool ContextMenu::getSizeToFitContent() const noexcept
{
    return false;
}

void ContextMenu::scrollToTop()
{
    POMDOG_ASSERT(scrollView_);
    scrollView_->scrollToTop();
}

void ContextMenu::scrollToEnd()
{
    POMDOG_ASSERT(scrollView_);
    scrollView_->scrollToEnd();
}

void ContextMenu::scrollTo(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(scrollView_);
    scrollView_->scrollTo(widget);
}

void ContextMenu::onEnter()
{
    POMDOG_ASSERT(scrollView_ != nullptr);
    scrollView_->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    scrollView_->setParent(shared_from_this());
    scrollView_->onEnter();
}

void ContextMenu::addChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->addChild(widget);
    needToUpdateLayout_ = true;
}

std::shared_ptr<Widget> ContextMenu::getChildAt(const Point2D& position)
{
    POMDOG_ASSERT(scrollView_ != nullptr);
    auto bounds = scrollView_->getBounds();
    if (bounds.contains(position)) {
        return scrollView_;
    }
    return nullptr;
}

void ContextMenu::updateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(scrollView_ != nullptr);
    scrollView_->updateAnimation(frameDuration);
}

void ContextMenu::updateLayout()
{
    POMDOG_ASSERT(scrollView_);
    scrollView_->doLayout();

    if (!needToUpdateLayout_) {
        return;
    }

    const auto scrollViewHeight = std::max(30, getHeight() - (padding_.top + padding_.bottom));

    if (scrollViewHeight != scrollView_->getHeight()) {
        scrollView_->setSize(scrollView_->getWidth(), scrollViewHeight);
        scrollView_->doLayout();
    }

    const auto requiredHeight = padding_.top + scrollView_->getHeight() + padding_.bottom;
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
        scrollView_->setPosition(Point2D{padding_.left, padding_.bottom});

        switch (scrollView_->getHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = getWidth() - (padding_.left + padding_.right);
            scrollView_->setSize(childWidth, scrollView_->getHeight());
            scrollView_->markContentLayoutDirty();
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

void ContextMenu::doLayout()
{
    updateLayout();
}

void ContextMenu::draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    const auto w = static_cast<float>(getWidth());
    const auto h = static_cast<float>(getHeight());

    primitiveBatch->drawRectangle(
        Matrix3x2::createIdentity(),
        math::toVector2(globalPos) + Vector2{-4.0f, -4.0f},
        w + 8.0f,
        h + 7.0f,
        colorScheme->ContextMenuShadowColor);

    primitiveBatch->drawRectangle(
        Matrix3x2::createIdentity(),
        math::toVector2(globalPos) + Vector2{-1.5f, -1.5f},
        w + 3.0f,
        h + 3.0f,
        colorScheme->ContextMenuShadowColor);

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x - 1, globalPos.y - 1, getWidth() + 2, getHeight() + 2},
        colorScheme->ContextMenuOutlineColor);

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
        colorScheme->ContextMenuBorderColor);

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x + 1, globalPos.y + 1, getWidth() - 2, getHeight() - 2},
        colorScheme->ContextMenuBackgroundColor);

    drawingContext.flushPrimitiveBatch();

    drawingContext.pushTransform(globalPos);

    POMDOG_ASSERT(scrollView_);
    scrollView_->draw(drawingContext);

    drawingContext.popTransform();
}

} // namespace pomdog::gui
