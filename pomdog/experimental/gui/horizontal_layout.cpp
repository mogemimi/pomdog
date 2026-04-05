// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"

namespace pomdog::gui {

HorizontalLayout::HorizontalLayout(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , margin_{0, 0, 0, 0}
    , layoutSpacing_(2)
    , needToUpdateLayout_(true)
    , isStackedLayout_(false)
{
    setSize(widthIn, heightIn);
    setInteractable(false);
}

void HorizontalLayout::setMargin(const Thickness& marginIn)
{
    margin_ = marginIn;
    needToUpdateLayout_ = true;
}

void HorizontalLayout::setLayoutSpacing(int spacingIn)
{
    layoutSpacing_ = spacingIn;
    needToUpdateLayout_ = true;
}

void HorizontalLayout::setStackedLayout(bool isStackedLayoutIn)
{
    if (isStackedLayout_ == isStackedLayoutIn) {
        return;
    }

    isStackedLayout_ = isStackedLayoutIn;
    needToUpdateLayout_ = true;
}

void HorizontalLayout::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    for (auto& child : children_) {
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->markParentTransformDirty();
    }
}

void HorizontalLayout::markParentTransformDirty()
{
    Widget::markParentTransformDirty();
    for (auto& child : children_) {
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->markParentTransformDirty();
    }
}

void HorizontalLayout::markContentLayoutDirty()
{
    needToUpdateLayout_ = true;
}

bool HorizontalLayout::getSizeToFitContent() const noexcept
{
    return false;
}

void HorizontalLayout::onEnter()
{
}

void HorizontalLayout::addChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget->getParent() == nullptr);

    Item item;
    item.widget = widget;
    item.stretch = 0;
    children_.push_back(std::move(item));

    widget->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    widget->setParent(shared_from_this());
    widget->onEnter();

    needToUpdateLayout_ = true;
}

std::shared_ptr<Widget> HorizontalLayout::getChildAt(const Point2D& position)
{
    for (auto& child : children_) {
        POMDOG_ASSERT(child.widget != nullptr);
        auto bounds = child.widget->getBounds();
        if (bounds.contains(position)) {
            return child.widget;
        }
    }
    return nullptr;
}

void HorizontalLayout::updateAnimation(const Duration& frameDuration)
{
    for (auto& child : children_) {
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->updateAnimation(frameDuration);
    }
}

void HorizontalLayout::setStretchFactor(int index, int stretch)
{
    POMDOG_ASSERT(stretch >= 0);
    POMDOG_ASSERT(!children_.empty());

    if (index < 0) {
        return;
    }
    if (index < static_cast<int>(children_.size())) {
        auto& child = children_[static_cast<size_t>(index)];
        child.stretch = stretch;
    }
}

void HorizontalLayout::setStretchFactor(const std::shared_ptr<Widget>& widget, int stretch)
{
    POMDOG_ASSERT(stretch >= 0);
    POMDOG_ASSERT(!children_.empty());
    POMDOG_ASSERT(widget->getParent() == shared_from_this());

    if (children_.empty()) {
        return;
    }
    if (widget->getParent() != shared_from_this()) {
        return;
    }

    auto iter = std::find_if(
        std::begin(children_),
        std::end(children_),
        [&](const auto& c) -> bool { return c.widget == widget; });

    if (iter != std::end(children_)) {
        iter->stretch = stretch;
    }
}

void HorizontalLayout::updateLayout()
{
    bool hasStretchFactor = false;
    int totalStretchFactor = 0;
    for (auto& child : children_) {
        child.widget->doLayout();
        if (child.stretch > 1) {
            hasStretchFactor = true;
        }
        totalStretchFactor += std::max(1, child.stretch);
    }

    if (!needToUpdateLayout_) {
        return;
    }

    {
        int maxHeight = 0;
        for (auto& child : children_) {
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

    if (children_.empty()) {
        needToUpdateLayout_ = false;
        return;
    }

    POMDOG_ASSERT(!children_.empty());
    const auto lineSpacingAll = (layoutSpacing_ * std::max(0, static_cast<int>(children_.size()) - 1));
    const auto horizontalSpacing = (margin_.left + margin_.right + lineSpacingAll);
    const auto containerWidth = std::max(1, (getWidth() - horizontalSpacing));
    const auto cellWidth = [&]() -> int {
        if (!isStackedLayout_) {
            return containerWidth / static_cast<int>(children_.size());
        }

        int childrenCount = 0;
        int fixedWidth = 0;
        for (auto& child : children_) {
            if (child.widget->getHorizontalAlignment() == HorizontalAlignment::Stretch) {
                ++childrenCount;
            }
            else {
                fixedWidth += child.widget->getWidth();
            }
        }

        POMDOG_ASSERT(childrenCount >= 0);
        if (childrenCount == 0) {
            return std::max(1, (getWidth() - horizontalSpacing) / static_cast<int>(children_.size()));
        }
        return (getWidth() - fixedWidth) / childrenCount;
    }();

    int offsetX = margin_.left;
    double inverseStretchFactor = 1.0 / totalStretchFactor;

    auto calcWidth = [&](int stretch) -> int {
        if (!hasStretchFactor) {
            return cellWidth;
        }
        return static_cast<int>(static_cast<double>(containerWidth * std::max(1, stretch)) * inverseStretchFactor);
    };

    // NOTE: update layout for children_
    for (auto& child : children_) {
        POMDOG_ASSERT(child.widget != nullptr);
        auto position = Point2D{offsetX, 0};

        switch (child.widget->getHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            child.widget->setSize(calcWidth(child.stretch), child.widget->getHeight());
            child.widget->markContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            if (!isStackedLayout_) {
                position.x = offsetX + cellWidth - child.widget->getWidth();
            }
            break;
        case HorizontalAlignment::Left:
            break;
        }

        if (child.widget->getHeight() < getHeight()) {
            position.y = (getHeight() - child.widget->getHeight()) / 2;
        }
        else {
            position.y = margin_.bottom;
        }

        child.widget->setPosition(position);
        if (isStackedLayout_) {
            offsetX += child.widget->getWidth();
        }
        else {
            offsetX += calcWidth(child.stretch);
        }
        offsetX += layoutSpacing_;
    }

    needToUpdateLayout_ = false;
}

void HorizontalLayout::doLayout()
{
    updateLayout();
}

void HorizontalLayout::draw(DrawingContext& drawingContext)
{
    updateLayout();
    POMDOG_ASSERT(!needToUpdateLayout_);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    drawingContext.pushTransform(globalPos);

    for (auto& child : children_) {
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->draw(drawingContext);
    }

    drawingContext.popTransform();
}

} // namespace pomdog::gui
