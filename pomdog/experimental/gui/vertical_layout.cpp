// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"

namespace pomdog::gui {

VerticalLayout::VerticalLayout(
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

void VerticalLayout::setMargin(const Thickness& marginIn)
{
    margin_ = marginIn;
    needToUpdateLayout_ = true;
}

void VerticalLayout::setLayoutSpacing(int spacingIn)
{
    layoutSpacing_ = spacingIn;
    needToUpdateLayout_ = true;
}

void VerticalLayout::setStackedLayout(bool isStackedLayoutIn)
{
    if (isStackedLayout_ == isStackedLayoutIn) {
        return;
    }

    isStackedLayout_ = isStackedLayoutIn;
    needToUpdateLayout_ = true;
}

void VerticalLayout::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);
    for (auto& child : children_) {
        POMDOG_ASSERT(child);
        child->markParentTransformDirty();
    }
}

void VerticalLayout::markParentTransformDirty()
{
    Widget::markParentTransformDirty();
    for (auto& child : children_) {
        POMDOG_ASSERT(child);
        child->markParentTransformDirty();
    }
}

void VerticalLayout::markContentLayoutDirty()
{
    needToUpdateLayout_ = true;
}

bool VerticalLayout::getSizeToFitContent() const noexcept
{
    return false;
}

void VerticalLayout::onEnter()
{
}

void VerticalLayout::addChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(!widget->getParent());

    children_.push_back(widget);

    widget->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    widget->setParent(shared_from_this());
    widget->onEnter();

    needToUpdateLayout_ = true;
}

void VerticalLayout::removeChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(!widget->getParent());

    auto iter = std::find(std::begin(children_), std::end(children_), widget);
    if (iter == std::end(children_)) {
        return;
    }
    children_.erase(iter);

    needToUpdateLayout_ = true;
}

void VerticalLayout::removeChild(int index)
{
    if (index >= static_cast<int>(children_.size())) {
        return;
    }
    auto iter = std::next(std::begin(children_), index);
    if (iter == std::end(children_)) {
        return;
    }
    children_.erase(iter);

    needToUpdateLayout_ = true;
}

std::shared_ptr<Widget> VerticalLayout::getChildAt(const Point2D& position)
{
    for (auto& child : children_) {
        POMDOG_ASSERT(child != nullptr);
        auto bounds = child->getBounds();
        if (bounds.contains(position)) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<Widget> VerticalLayout::getChildAt(int index)
{
    POMDOG_ASSERT(index < static_cast<int>(children_.size()));
    return children_.at(static_cast<std::size_t>(index));
}

int VerticalLayout::getItemCount() const noexcept
{
    return static_cast<int>(children_.size());
}

void VerticalLayout::updateAnimation(const Duration& frameDuration)
{
    for (auto& child : children_) {
        POMDOG_ASSERT(child != nullptr);
        child->updateAnimation(frameDuration);
    }
}

void VerticalLayout::updateLayout()
{
    for (auto& child : children_) {
        child->doLayout();
    }

    if (!needToUpdateLayout_) {
        return;
    }

    if (children_.empty()) {
        // NOTE: Resizing this panel
        setSize(getWidth(), 0);

        if (auto parent = getParent(); parent != nullptr) {
            parent->markContentLayoutDirty();
        }
        needToUpdateLayout_ = false;
        return;
    }

    POMDOG_ASSERT(!children_.empty());
    const auto lineSpacingAll = (layoutSpacing_ * std::max(0, static_cast<int>(children_.size()) - 1));
    const auto verticalSpacing = (margin_.top + margin_.bottom + lineSpacingAll);

    int requiredHeight = verticalSpacing;
    for (auto& child : children_) {
        requiredHeight += child->getHeight();
    }

    if (requiredHeight != getHeight()) {
        // NOTE: Resizing this panel
        setSize(getWidth(), requiredHeight);

        if (auto parent = getParent(); parent != nullptr) {
            parent->markContentLayoutDirty();
        }
    }

    // NOTE: update layout for children_
    int offsetY = margin_.top;
    for (auto& child : children_) {
        offsetY += child->getHeight();
        auto position = Point2D{margin_.left, getHeight() - offsetY};

        switch (child->getHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = getWidth() - (margin_.left + margin_.right);
            child->setSize(childWidth, child->getHeight());
            child->markContentLayoutDirty();
            break;
        }
        case HorizontalAlignment::Right: {
            position.x = getWidth() - margin_.right - child->getWidth();
            break;
        }
        case HorizontalAlignment::Left:
            break;
        }

        child->setPosition(position);
        offsetY += layoutSpacing_;
    }

    needToUpdateLayout_ = false;
}

void VerticalLayout::doLayout()
{
    updateLayout();
}

void VerticalLayout::draw(DrawingContext& drawingContext)
{
    updateLayout();
    POMDOG_ASSERT(!needToUpdateLayout_);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    drawingContext.pushTransform(globalPos);

    for (auto& child : children_) {
        POMDOG_ASSERT(child);
        child->draw(drawingContext);
    }

    drawingContext.popTransform();
}

} // namespace pomdog::gui
