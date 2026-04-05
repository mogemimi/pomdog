// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/widget.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gui {

Widget::Widget(const std::shared_ptr<UIEventDispatcher>& dispatcherIn)
    : localPosition_(0, 0)
    , parentPosition_(0, 0)
    , weakDispatcher_(dispatcherIn)
    , height_(1)
    , width_(1)
    , hierarchySortOrder_(HierarchySortOrder::Sortable)
    , isParentTransformDirty_(true)
    , isVisible_(true)
    , isInteractable_(true)
    , isWheelFocusEnabled_(false)
{
}

Widget::~Widget() = default;

std::shared_ptr<UIEventDispatcher> Widget::getDispatcher() const
{
    return weakDispatcher_.lock();
}

void Widget::setParent(const std::shared_ptr<Widget>& parentIn)
{
    weakParent_ = parentIn;
    POMDOG_ASSERT(weakDispatcher_.lock() == parentIn->getDispatcher());
}

std::shared_ptr<Widget const> Widget::getParent() const
{
    return weakParent_.lock();
}

std::shared_ptr<Widget> Widget::getParent()
{
    return weakParent_.lock();
}

int Widget::getX() const noexcept
{
    return localPosition_.x;
}

int Widget::getY() const noexcept
{
    return localPosition_.y;
}

int Widget::getWidth() const noexcept
{
    return width_;
}

int Widget::getHeight() const noexcept
{
    return height_;
}

void Widget::setSize(int widthIn, int heightIn)
{
    width_ = widthIn;
    height_ = heightIn;
}

Rect2D Widget::getBounds() const noexcept
{
    return Rect2D{localPosition_.x, localPosition_.y, width_, height_};
}

void Widget::markParentTransformDirty()
{
    isParentTransformDirty_ = true;
}

Point2D Widget::getGlobalPosition()
{
    if (isParentTransformDirty_) {
        if (auto widget = weakParent_.lock(); widget != nullptr) {
            parentPosition_ = widget->getGlobalPosition();
        }
        else {
            parentPosition_ = Point2D{0, 0};
        }
        isParentTransformDirty_ = false;
    }

    POMDOG_ASSERT(!isParentTransformDirty_);
    return localPosition_ + parentPosition_;
}

bool Widget::isVisible() const noexcept
{
    return isVisible_;
}

void Widget::setVisible(bool visible) noexcept
{
    isVisible_ = visible;
}

bool Widget::isInteractable() const noexcept
{
    return isInteractable_;
}

void Widget::setInteractable(bool interactable) noexcept
{
    isInteractable_ = interactable;
}

bool Widget::isWheelFocusEnabled() const noexcept
{
    return isWheelFocusEnabled_;
}

void Widget::setWheelFocusEnabled(bool wheelFocusEnabled) noexcept
{
    isWheelFocusEnabled_ = wheelFocusEnabled;
}

HierarchySortOrder Widget::getHierarchySortOrder() const noexcept
{
    return hierarchySortOrder_;
}

void Widget::setHierarchySortOrder(HierarchySortOrder sortOrder) noexcept
{
    hierarchySortOrder_ = sortOrder;
}

Point2D Widget::getPosition() const noexcept
{
    return localPosition_;
}

void Widget::setPosition(const Point2D& positionIn)
{
    localPosition_ = positionIn;
}

void Widget::markContentLayoutDirty()
{
}

void Widget::doLayout()
{
}

std::shared_ptr<Widget> Widget::getChildAt([[maybe_unused]] const Point2D& positionIn)
{
    return nullptr;
}

bool Widget::getSizeToFitContent() const noexcept
{
    return false;
}

HorizontalAlignment Widget::getHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Stretch;
}

VerticalAlignment Widget::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Stretch;
}

void Widget::onEnter()
{
}

void Widget::onFocusIn()
{
}

void Widget::onFocusOut()
{
}

void Widget::onTextInput(const KeyboardState&, const std::string&)
{
}

void Widget::onKeyDown(const KeyboardState&, Keys)
{
}

void Widget::onKeyUp(const KeyboardState&, Keys)
{
}

void Widget::onPointerCanceled(const PointerPoint&)
{
}

void Widget::onPointerCaptureLost(const PointerPoint&)
{
}

void Widget::onPointerWheelChanged(const PointerPoint&)
{
}

void Widget::onPointerEntered(const PointerPoint&)
{
}

void Widget::onPointerExited(const PointerPoint&)
{
}

void Widget::onPointerPressed(const PointerPoint&)
{
}

void Widget::onPointerMoved(const PointerPoint&)
{
}

void Widget::onPointerReleased(const PointerPoint&)
{
}

void Widget::draw(DrawingContext&)
{
}

void Widget::updateAnimation(const Duration&)
{
}

void Widget::setCursor(MouseCursor cursorIn)
{
    cursor_ = cursorIn;
}

void Widget::resetCursor()
{
    cursor_ = std::nullopt;
}

std::optional<MouseCursor> Widget::getCurrentCursor() const
{
    return cursor_;
}

} // namespace pomdog::gui
