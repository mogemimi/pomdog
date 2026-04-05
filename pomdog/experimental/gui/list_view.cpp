// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/list_view.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/list_view_item.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/experimental/gui/vertical_layout.h"

namespace pomdog::gui {

ListView::ListView(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , padding_{4, 1, 4, 1}
    , needToUpdateLayout_(true)
{
    setSize(widthIn, heightIn);

    verticalLayout_ = std::make_shared<VerticalLayout>(dispatcher, 140, 10);
    verticalLayout_->setStackedLayout(true);
    verticalLayout_->setLayoutSpacing(0);

    setHierarchySortOrder(HierarchySortOrder::Front);
}

void ListView::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->markParentTransformDirty();
}

void ListView::setPadding(const Thickness& paddingIn)
{
    padding_ = paddingIn;
    needToUpdateLayout_ = true;
}

void ListView::markParentTransformDirty()
{
    Widget::markParentTransformDirty();

    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->markParentTransformDirty();
}

void ListView::markContentLayoutDirty()
{
    needToUpdateLayout_ = true;
}

bool ListView::getSizeToFitContent() const noexcept
{
    return false;
}

void ListView::onEnter()
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    verticalLayout_->setParent(shared_from_this());
    verticalLayout_->onEnter();
}

void ListView::addChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);

    auto itemCount = verticalLayout_->getItemCount();

    auto dispatcher = getDispatcher();
    auto item = std::make_shared<ListViewItem>(dispatcher);
    item->setWidget(widget);
    item->setListViewIndex(itemCount);

    verticalLayout_->addChild(item);
    needToUpdateLayout_ = true;
}

void ListView::removeChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->removeChild(widget);

    // NOTE: Reindexing
    const auto itemCount = verticalLayout_->getItemCount();
    for (int i = 0; i < itemCount; i++) {
        auto item = std::static_pointer_cast<ListViewItem>(verticalLayout_->getChildAt(i));
        item->setListViewIndex(i);
    }

    needToUpdateLayout_ = true;
}

void ListView::removeChild(int index)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->removeChild(index);

    // NOTE: Reindexing
    const auto itemCount = verticalLayout_->getItemCount();
    for (int i = 0; i < itemCount; i++) {
        auto item = std::static_pointer_cast<ListViewItem>(verticalLayout_->getChildAt(i));
        item->setListViewIndex(i);
    }

    needToUpdateLayout_ = true;
}

int ListView::getItemCount() const noexcept
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    return verticalLayout_->getItemCount();
}

std::optional<int> ListView::getCurrentIndex() const noexcept
{
    return currentIndex_;
}

void ListView::setCurrentIndex(int index)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);

    if ((currentIndex_ != std::nullopt) &&
        (index != *currentIndex_) &&
        (*currentIndex_ < verticalLayout_->getItemCount())) {
        if (auto child = verticalLayout_->getChildAt(*currentIndex_); child != nullptr) {
            auto deselectedItem = std::dynamic_pointer_cast<ListViewItem>(child);
            POMDOG_ASSERT(deselectedItem);

            deselectedItem->setSelected(false);
        }
    }

    auto child = verticalLayout_->getChildAt(index);
    if (child == nullptr) {
        currentIndex_ = std::nullopt;
        return;
    }

    auto selectedItem = std::dynamic_pointer_cast<ListViewItem>(child);
    POMDOG_ASSERT(selectedItem);
    selectedItem->setSelected(true);

    currentIndex_ = index;
    CurrentIndexChanged(currentIndex_);
}

std::shared_ptr<Widget> ListView::getChildAt(const Point2D& position)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    auto bounds = verticalLayout_->getBounds();
    if (bounds.contains(position)) {
        return verticalLayout_;
    }
    return nullptr;
}

std::shared_ptr<Widget> ListView::getChildAt(int index)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    auto item = std::static_pointer_cast<ListViewItem>(verticalLayout_->getChildAt(index));
    return item->getChild();
}

void ListView::updateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->updateAnimation(frameDuration);
}

void ListView::updateLayout()
{
    POMDOG_ASSERT(verticalLayout_ != nullptr);
    verticalLayout_->doLayout();

    if (!needToUpdateLayout_) {
        return;
    }

    const auto requiredHeight = padding_.top + verticalLayout_->getHeight() + padding_.bottom;
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

void ListView::doLayout()
{
    updateLayout();
}

void ListView::draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    if (colorScheme->ListViewBackgroundColorOdd.a > 0) {
        primitiveBatch->drawRectangle(
            Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
            colorScheme->ListViewBackgroundColorOdd);
    }

    if (colorScheme->ListViewBackgroundColorEven.a > 0) {
        const auto itemCount = verticalLayout_->getItemCount();
        for (int i = 0; i < itemCount; i++) {
            if (i % 2 == 0) {
                continue;
            }
            auto child = verticalLayout_->getChildAt(i);
            auto childPos = child->getGlobalPosition();

            primitiveBatch->drawRectangle(
                Rect2D{globalPos.x, childPos.y, getWidth(), child->getHeight()},
                colorScheme->ListViewBackgroundColorEven);
        }
    }

    primitiveBatch->flush();

    drawingContext.pushTransform(globalPos);

    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->draw(drawingContext);

    drawingContext.popTransform();
}

} // namespace pomdog::gui
