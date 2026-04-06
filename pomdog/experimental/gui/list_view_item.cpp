// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/list_view_item.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/list_view.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

ListViewItem::ListViewItem(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , listViewIndex_(0)
    , marginLeft_(10)
    , marginRight_(10)
    , horizontalAlignment_(HorizontalAlignment::Stretch)
    , verticalAlignment_(VerticalAlignment::Top)
    , isEnabled_(true)
    , isHovered_(false)
    , isFocused_(false)
    , isSelected_(false)
{
    setSize(74, 20);
}

bool ListViewItem::isEnabled() const noexcept
{
    return isEnabled_;
}

void ListViewItem::setEnabled(bool isEnabledIn)
{
    isEnabled_ = isEnabledIn;
}

bool ListViewItem::isHovered() const noexcept
{
    return isHovered_;
}

bool ListViewItem::isFocused() const noexcept
{
    return isFocused_;
}

bool ListViewItem::isSelected() const noexcept
{
    return isSelected_;
}

void ListViewItem::setSelected(bool selected)
{
    isSelected_ = selected;
}

int ListViewItem::getListViewIndex() const noexcept
{
    return listViewIndex_;
}

void ListViewItem::setListViewIndex(int index) noexcept
{
    listViewIndex_ = static_cast<i32>(index);
}

void ListViewItem::setWidget(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget != nullptr);
    POMDOG_ASSERT(!widget->getParent());

    child_ = widget;
    child_->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    child_->setParent(shared_from_this());
    child_->onEnter();

    constexpr auto marginBottom = 6;
    child_->setPosition(Point2D{marginLeft_, marginBottom});
}

void ListViewItem::setMargin(const Thickness& margin)
{
    static_assert(std::is_same_v<decltype(marginLeft_), i16>);
    static_assert(std::is_same_v<decltype(marginRight_), i16>);

    marginLeft_ = static_cast<i16>(margin.left);
    marginRight_ = static_cast<i16>(margin.right);
}

void ListViewItem::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    horizontalAlignment_ = horizontalAlignmentIn;
}

void ListViewItem::setVerticalAlignment(VerticalAlignment verticalAlignmentIn) noexcept
{
    verticalAlignment_ = verticalAlignmentIn;
}

HorizontalAlignment ListViewItem::getHorizontalAlignment() const noexcept
{
    return horizontalAlignment_;
}

VerticalAlignment ListViewItem::getVerticalAlignment() const noexcept
{
    return verticalAlignment_;
}

void ListViewItem::onEnter()
{
}

void ListViewItem::onFocusIn()
{
    isFocused_ = true;
}

void ListViewItem::onFocusOut()
{
    isFocused_ = false;
}

void ListViewItem::onPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isHovered_ = true;
}

void ListViewItem::onPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isHovered_ = false;
}

void ListViewItem::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void ListViewItem::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isSelected_ = true;

    auto parent = getParent();
    POMDOG_ASSERT(parent);
    POMDOG_ASSERT(std::dynamic_pointer_cast<VerticalLayout>(parent) != nullptr);

    auto listView = std::static_pointer_cast<ListView>(parent->getParent());
    POMDOG_ASSERT(std::dynamic_pointer_cast<ListView>(parent->getParent()) != nullptr);
    POMDOG_ASSERT(listView);

    listView->setCurrentIndex(listViewIndex_);
}

std::shared_ptr<Widget> ListViewItem::getChildAt(const Point2D& position)
{
    if (child_ != nullptr) {
        auto bounds = child_->getBounds();
        if (bounds.contains(position)) {
            return child_;
        }
    }
    return nullptr;
}

std::shared_ptr<Widget> ListViewItem::getChild()
{
    return child_;
}

void ListViewItem::updateAnimation(const Duration& frameDuration)
{
    if (child_ != nullptr) {
        child_->updateAnimation(frameDuration);
    }
}

void ListViewItem::doLayout()
{
    if (child_ != nullptr) {
        child_->doLayout();
    }
}

void ListViewItem::draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    if (isEnabled_ && (isHovered_ || isSelected_)) {
        auto color = colorScheme->ListViewItemColorHovered;

        if (isSelected_) {
            if (isFocused_) {
                color = colorScheme->ListViewItemColorActive;
            }
            else {
                color = colorScheme->ListViewItemColorSelected;
            }
        }

        primitiveBatch->drawRectangle(
            Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
            color);

        drawingContext.flushPrimitiveBatch();
    }

    if (child_ != nullptr) {
        drawingContext.pushTransform(globalPos);

        if (child_ != nullptr) {
            child_->draw(drawingContext);
        }

        drawingContext.popTransform();
    }
}

} // namespace pomdog::gui
