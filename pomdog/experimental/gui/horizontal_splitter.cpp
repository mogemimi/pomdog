// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/horizontal_splitter.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

class HorizontalSplitterHandle final
    : public Widget
    , public std::enable_shared_from_this<HorizontalSplitterHandle> {
public:
    explicit HorizontalSplitterHandle(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    [[nodiscard]] bool
    isEnabled() const noexcept;

    void
    setEnabled(bool enabled) noexcept;

    [[nodiscard]] bool
    isVisible() const noexcept;

    void
    setVisible(bool visible) noexcept;

    void
    setColor(const Color&) noexcept;

    void
    setBorderWidth(int width) noexcept;

    [[nodiscard]] int
    getBorderWidth() const noexcept;

    [[nodiscard]] int
    getBorderWidthOffset() const noexcept;

    [[nodiscard]] Point2D
    getBorderPosition() const noexcept;

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    onPointerPressed(const PointerPoint& pointerPoint) override;

    void
    onPointerMoved(const PointerPoint& pointerPoint) override;

    void
    onPointerReleased(const PointerPoint& pointerPoint) override;

    void
    draw(DrawingContext& drawingContext) override;

    Signal<void(int)> Resizing;

private:
    std::optional<Point2D> grabStartPosition_;
    Color color_;
    int borderWidth_ = 1;
    bool isEnabled_;
    bool isVisible_;
};

HorizontalSplitter::HorizontalSplitter(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , margin_{0, 0, 0, 0}
    , backgroundColor_(Color::createTransparentBlack())
    , needToUpdateLayout_(true)
{
    setSize(widthIn, heightIn);
    setInteractable(false);

    splitterHandle_ = std::make_shared<HorizontalSplitterHandle>(dispatcher);
    splitterHandle_->setBorderWidth(1);
    splitterHandle_->setSize(5, getHeight());
    splitterHandle_->setPosition(Point2D{getWidth() / 2, 0});
}

void HorizontalSplitter::setMargin(const Thickness& marginIn)
{
    margin_ = marginIn;
    needToUpdateLayout_ = true;
}

void HorizontalSplitter::setLayoutSpacing(int spacing)
{
    POMDOG_ASSERT(splitterHandle_);
    POMDOG_ASSERT(spacing >= 0);
    spacing = std::max(spacing, 0);

    constexpr int handleCollisionOffset = 4;
    const auto minimumCollisionWidth = handleCollisionOffset + (spacing % 2);

    splitterHandle_->setBorderWidth(spacing);
    splitterHandle_->setSize(std::max(spacing, minimumCollisionWidth), getHeight());
    if (childSplitter_) {
        childSplitter_->setLayoutSpacing(spacing);
    }
    needToUpdateLayout_ = true;
}

void HorizontalSplitter::setBackgroundColor(const Color& color)
{
    backgroundColor_ = color;
    if (childSplitter_) {
        childSplitter_->setBackgroundColor(color);
    }
}

void HorizontalSplitter::setHandleColor(const Color& color)
{
    POMDOG_ASSERT(splitterHandle_);
    splitterHandle_->setColor(color);
    if (childSplitter_) {
        childSplitter_->setHandleColor(color);
    }
}

void HorizontalSplitter::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    for (auto& child : children_) {
        if (child.widget == nullptr) {
            continue;
        }
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->markParentTransformDirty();
    }
}

void HorizontalSplitter::markParentTransformDirty()
{
    Widget::markParentTransformDirty();
    for (auto& child : children_) {
        if (child.widget == nullptr) {
            continue;
        }
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->markParentTransformDirty();
    }
}

void HorizontalSplitter::markContentLayoutDirty()
{
    needToUpdateLayout_ = true;
}

bool HorizontalSplitter::getSizeToFitContent() const noexcept
{
    return true;
}

void HorizontalSplitter::onEnter()
{
    splitterHandle_->setParent(shared_from_this());
    splitterHandle_->onEnter();

    resizingConn_ = splitterHandle_->Resizing.connect([this](int offset) {
        auto pos = splitterHandle_->getPosition();
        if (offset <= 0) {
            auto& left = children_[0];
            pos.x = std::max(pos.x + offset, left.minimumWidth);
        }
        else {
            auto& right = children_[1];
            pos.x = std::min(
                pos.x + offset,
                getWidth() - (right.minimumWidth + splitterHandle_->getBorderWidth()));
        }

        splitterHandle_->setPosition(pos);
        needToUpdateLayout_ = true;
    });
}

void HorizontalSplitter::addChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget->getParent() == nullptr);
    POMDOG_ASSERT(splitterHandle_);

    Item item;
    item.widget = widget;
    item.minimumWidth = 0;

    if (children_[0].widget == nullptr) {
        children_[0] = std::move(item);

        const auto offset = splitterHandle_->getBorderWidthOffset();
        splitterHandle_->setPosition(Point2D{widget->getWidth() - offset, 0});
    }
    else if (children_[1].widget == nullptr) {
        children_[1] = std::move(item);
    }
    else if (childSplitter_ == nullptr) {
        updateLayout();

        auto& right = children_[1];
        auto dispatcher = getDispatcher();
        auto w = std::max(right.widget->getWidth(), right.minimumWidth);
        auto h = getHeight();
        childSplitter_ = std::make_shared<HorizontalSplitter>(dispatcher, w, h);
        childSplitter_->setParent(shared_from_this());
        childSplitter_->onEnter();

        right.widget->setPosition(Point2D{0, 0});
        right.widget->setParent(childSplitter_);
        right.widget->markParentTransformDirty();

        childSplitter_->children_[0].widget = right.widget;
        childSplitter_->children_[0].minimumWidth = right.minimumWidth;
        childSplitter_->splitterHandle_->setPosition(Point2D{childSplitter_->getWidth() / 2, 0});
        childSplitter_->splitterHandle_->setSize(splitterHandle_->getWidth(), getHeight());
        childSplitter_->backgroundColor_ = backgroundColor_;

        childSplitter_->addChild(widget);

        right.widget = childSplitter_;
        needToUpdateLayout_ = true;
        return;
    }
    else {
        POMDOG_ASSERT(children_[0].widget != nullptr);
        POMDOG_ASSERT(children_[1].widget != nullptr);
        POMDOG_ASSERT(childSplitter_ != nullptr);
        childSplitter_->addChild(widget);
        needToUpdateLayout_ = true;
        return;
    }

    widget->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    widget->setParent(shared_from_this());
    widget->onEnter();

    needToUpdateLayout_ = true;
}

std::shared_ptr<Widget> HorizontalSplitter::getChildAt(const Point2D& position)
{
    {
        auto bounds = splitterHandle_->getBounds();
        if (bounds.contains(position)) {
            return splitterHandle_;
        }
    }

    for (auto& child : children_) {
        if (child.widget == nullptr) {
            continue;
        }
        auto bounds = child.widget->getBounds();
        if (bounds.contains(position)) {
            return child.widget;
        }
    }
    return nullptr;
}

void HorizontalSplitter::updateAnimation(const Duration& frameDuration)
{
    for (auto& child : children_) {
        if (child.widget == nullptr) {
            continue;
        }
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->updateAnimation(frameDuration);
    }
}

void HorizontalSplitter::setMinimumWidth(int index, int minimumWidth)
{
    POMDOG_ASSERT(minimumWidth >= 0);
    POMDOG_ASSERT(!children_.empty());

    if (index < 0) {
        return;
    }
    const auto count = static_cast<int>(children_.size());
    if (index < count) {
        auto& child = children_[static_cast<size_t>(index)];
        child.minimumWidth = minimumWidth;
    }
    else if (childSplitter_ != nullptr) {
        POMDOG_ASSERT(index >= static_cast<int>(children_.size()));
        childSplitter_->setMinimumWidth(index - count, minimumWidth);
    }
}

void HorizontalSplitter::setMinimumWidth(const std::shared_ptr<Widget>& widget, int minimumWidth)
{
    POMDOG_ASSERT(minimumWidth >= 0);
    POMDOG_ASSERT(!children_.empty());

    if (widget->getParent() != shared_from_this()) {
        if (childSplitter_ != nullptr) {
            childSplitter_->setMinimumWidth(widget, minimumWidth);
            auto& right = children_[1];
            right.minimumWidth = splitterHandle_->getBorderWidth();
            for (auto& child : childSplitter_->children_) {
                right.minimumWidth += child.minimumWidth;
            }
        }
        return;
    }

    POMDOG_ASSERT(widget->getParent() == shared_from_this());

    auto iter = std::find_if(
        std::begin(children_),
        std::end(children_),
        [&](const auto& c) -> bool { return c.widget == widget; });

    if (iter != std::end(children_)) {
        iter->minimumWidth = minimumWidth;
    }
}

void HorizontalSplitter::updateLayout()
{
    if (childSplitter_ != nullptr) {
        childSplitter_->setSize(childSplitter_->getWidth(), getHeight());
    }

    for (auto& child : children_) {
        if (child.widget != nullptr) {
            child.widget->doLayout();
        }
    }

    if (!needToUpdateLayout_) {
        return;
    }

    {
        int maxHeight = 0;
        for (auto& child : children_) {
            if (child.widget == nullptr) {
                continue;
            }
            if (child.widget->getVerticalAlignment() == VerticalAlignment::Stretch) {
                continue;
            }
            if (child.widget->getSizeToFitContent()) {
                continue;
            }
            maxHeight = std::max(child.widget->getHeight(), maxHeight);
        }

        const auto requiredHeight = maxHeight + margin_.top + margin_.bottom;
        if (requiredHeight > getHeight()) {
            // NOTE: Resizing this panel
            setSize(getWidth(), requiredHeight);

            auto parent = getParent();
            if (parent) {
                parent->markContentLayoutDirty();
            }
        }
    }

    {
        auto position = splitterHandle_->getPosition();
        position.x = std::clamp(
            position.x,
            0,
            getWidth() - (children_[1].minimumWidth + splitterHandle_->getBorderWidth()));
        splitterHandle_->setPosition(position);
        splitterHandle_->setSize(splitterHandle_->getWidth(), getHeight());
    }

    POMDOG_ASSERT(!children_.empty());

    int offsetX = margin_.left;

    // NOTE: update layout for children_
    for (int i = 0; i < static_cast<int>(children_.size()); i++) {
        auto& child = children_[i];
        if (child.widget == nullptr) {
            break;
        }

        auto handle = splitterHandle_;
        if (i > 0) {
            handle = nullptr;
        }

        auto position = Point2D{offsetX, 0};
        auto requiredWidth = [&]() -> int {
            if (handle == nullptr) {
                return getWidth() - offsetX;
            }
            auto handlePosition = handle->getBorderPosition();
            return std::max(child.minimumWidth, handlePosition.x - position.x);
        }();

        switch (child.widget->getHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto h = child.widget->getHeight();
            if (child.widget->getVerticalAlignment() == VerticalAlignment::Stretch) {
                h = getHeight() - (margin_.bottom + margin_.top);
            }
            child.widget->setSize(requiredWidth, h);
            child.widget->markContentLayoutDirty();
            break;
        }
        case HorizontalAlignment::Right:
            position.x = offsetX + requiredWidth - child.widget->getWidth();
            break;
        case HorizontalAlignment::Left:
            break;
        }

        switch (child.widget->getVerticalAlignment()) {
        case VerticalAlignment::Stretch: {
            position.y = margin_.bottom;
            break;
        }
        case VerticalAlignment::Top:
            position.y = getHeight() - child.widget->getHeight() - margin_.top;
            break;
            // case VerticalAlignment::Center:
            //     position.Y = (getHeight() - child.widget->getHeight()) / 2;
            //     break;
        }

        child.widget->setPosition(position);

        if (handle != nullptr) {
            auto handlePosition = handle->getPosition();
            offsetX = handlePosition.x + handle->getBorderWidth();
        }
    }

    needToUpdateLayout_ = false;
}

void HorizontalSplitter::doLayout()
{
    updateLayout();
}

void HorizontalSplitter::draw(DrawingContext& drawingContext)
{
    updateLayout();
    POMDOG_ASSERT(!needToUpdateLayout_);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    if (backgroundColor_.a > 0) {
        auto primitiveBatch = drawingContext.getPrimitiveBatch();

        primitiveBatch->drawRectangle(
            Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
            backgroundColor_);

        primitiveBatch->flush();
    }

    drawingContext.pushTransform(globalPos);

    for (auto& child : children_) {
        if (child.widget == nullptr) {
            continue;
        }
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->draw(drawingContext);
    }

    splitterHandle_->draw(drawingContext);

    drawingContext.popTransform();
}

HorizontalSplitterHandle::HorizontalSplitterHandle(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , color_(Color{45, 45, 48, 255})
    , borderWidth_(1)
    , isEnabled_(true)
    , isVisible_(true)
{
    setSize(10, 1);
    setCursor(MouseCursor::ResizeHorizontal);
}

bool HorizontalSplitterHandle::isEnabled() const noexcept
{
    return isEnabled_;
}

void HorizontalSplitterHandle::setEnabled(bool enabledIn) noexcept
{
    isEnabled_ = enabledIn;
}

bool HorizontalSplitterHandle::isVisible() const noexcept
{
    return isVisible_;
}

void HorizontalSplitterHandle::setVisible(bool visibleIn) noexcept
{
    isVisible_ = visibleIn;
}

void HorizontalSplitterHandle::setColor(const Color& colorIn) noexcept
{
    color_ = colorIn;
}

void HorizontalSplitterHandle::setBorderWidth(int widthIn) noexcept
{
    POMDOG_ASSERT(widthIn >= 0);
    borderWidth_ = widthIn;

    if (borderWidth_ > getWidth()) {
        setSize(borderWidth_, getHeight());
    }
}

int HorizontalSplitterHandle::getBorderWidth() const noexcept
{
    return borderWidth_;
}

int HorizontalSplitterHandle::getBorderWidthOffset() const noexcept
{
    return std::max((getWidth() - borderWidth_) / 2, 0);
}

Point2D HorizontalSplitterHandle::getBorderPosition() const noexcept
{
    auto pos = getPosition();
    pos.x = pos.x + getBorderWidthOffset();
    return pos;
}

HorizontalAlignment HorizontalSplitterHandle::getHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Stretch;
}

VerticalAlignment HorizontalSplitterHandle::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void HorizontalSplitterHandle::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());
    grabStartPosition_ = pointInView;
}

void HorizontalSplitterHandle::onPointerMoved(const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (grabStartPosition_) {
        auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());
        Resizing(pointInView.x - grabStartPosition_->x);
    }
}

void HorizontalSplitterHandle::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    grabStartPosition_ = std::nullopt;
}

void HorizontalSplitterHandle::draw(DrawingContext& drawingContext)
{
    if (!isVisible_) {
        return;
    }

    auto globalPos = UIHelper::projectToWorldSpace(getBorderPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x, globalPos.y, borderWidth_, getHeight()},
        color_);

    primitiveBatch->flush();
}

} // namespace pomdog::gui
