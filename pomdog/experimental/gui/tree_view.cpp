// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/tree_view.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/font_size.h"
#include "pomdog/experimental/gui/font_weight.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

TreeView::TreeView(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , layoutSpacing_(6)
    , indentation_(14)
    , expanded_(true)
    , needToUpdateLayout_(true)
{
    setSize(50, 12);

    horizontalLayout_ = std::make_shared<HorizontalLayout>(dispatcher, 140, 10);
    horizontalLayout_->setStackedLayout(true);
    horizontalLayout_->setLayoutSpacing(5);

    disclosureTriangleButton_ = std::make_shared<DisclosureTriangleButton>(dispatcher);
    horizontalLayout_->addChild(disclosureTriangleButton_);

    textBlock_ = std::make_shared<TextBlock>(dispatcher);
    textBlock_->setFontWeight(FontWeight::Bold);
    textBlock_->setColor(Color{252, 251, 248, 255});
    textBlock_->setBaselineOffset(2.0f);
    horizontalLayout_->addChild(textBlock_);

    verticalLayout_ = std::make_shared<VerticalLayout>(dispatcher, 140, 10);
    verticalLayout_->setLayoutSpacing(8);
    verticalLayout_->setMargin(Thickness{2, 0, 4, 0});

    connection_ = disclosureTriangleButton_->Toggled.connect([this](bool isOn) {
        this->setExpanded(isOn);
    });
}

void TreeView::setText(const std::string& text)
{
    textBlock_->setText(text);
}

void TreeView::setExpanded(bool expandedIn)
{
    if (expanded_ == expandedIn) {
        return;
    }
    expanded_ = expandedIn;
    needToUpdateLayout_ = true;
}

void TreeView::setVerticalLayoutSpacing(int spacing)
{
    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->setLayoutSpacing(spacing);
}

void TreeView::onEnter()
{
    std::array<std::shared_ptr<Widget>, 2> children = {{horizontalLayout_, verticalLayout_}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->markParentTransformDirty();

        POMDOG_ASSERT(shared_from_this());
        child->setParent(shared_from_this());
        child->onEnter();
    }
}

HorizontalAlignment TreeView::getHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Stretch;
}

VerticalAlignment TreeView::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void TreeView::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);
    verticalLayout_->markParentTransformDirty();
    horizontalLayout_->markParentTransformDirty();
}

void TreeView::markParentTransformDirty()
{
    Widget::markParentTransformDirty();
    verticalLayout_->markParentTransformDirty();
    horizontalLayout_->markParentTransformDirty();
}

void TreeView::markContentLayoutDirty()
{
    needToUpdateLayout_ = true;
}

void TreeView::addChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout_);
    verticalLayout_->addChild(widget);
}

std::shared_ptr<Widget> TreeView::getChildAt(const Point2D& position)
{
    std::array<std::shared_ptr<Widget>, 3> children = {{
        horizontalLayout_,
        disclosureTriangleButton_,
        verticalLayout_,
    }};

    if (!expanded_) {
        children[2] = nullptr;
    }

    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }
        auto bounds = child->getBounds();
        if (bounds.contains(position)) {
            return child;
        }
    }
    return nullptr;
}

void TreeView::updateAnimation(const Duration& frameDuration)
{
    std::array<std::shared_ptr<Widget>, 3> children = {{
        horizontalLayout_,
        disclosureTriangleButton_,
        verticalLayout_,
    }};

    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }
        child->updateAnimation(frameDuration);
    }
}

void TreeView::updateLayout()
{
    verticalLayout_->doLayout();
    horizontalLayout_->doLayout();

    if (!needToUpdateLayout_) {
        return;
    }

    disclosureTriangleButton_->setOn(expanded_);

    const auto verticalLayoutHeight = verticalLayout_->getHeight();
    const auto horizontalLayoutHeight = horizontalLayout_->getHeight();

    {
        auto requiredHeight = horizontalLayoutHeight;
        if (expanded_) {
            requiredHeight += layoutSpacing_;
            requiredHeight += verticalLayoutHeight;
        }

        if (requiredHeight != getHeight()) {
            // NOTE: Resizing this panel
            setSize(std::max(getWidth(), indentation_ + 1), requiredHeight);
            if (auto parent = getParent()) {
                parent->markContentLayoutDirty();
            }
        }
    }
    {
        auto position = Point2D{0, 0};
        switch (horizontalLayout_->getHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            horizontalLayout_->setSize(getWidth(), horizontalLayout_->getHeight());
            horizontalLayout_->markContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            position.x = getWidth() - horizontalLayout_->getWidth();
            break;
        case HorizontalAlignment::Left:
            break;
        }
        if (expanded_) {
            position.y += layoutSpacing_;
            position.y += verticalLayoutHeight;
        }
        horizontalLayout_->setPosition(position);
    }
    {
        POMDOG_ASSERT(indentation_ < getWidth());

        auto position = Point2D{indentation_, 0};
        switch (verticalLayout_->getHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            verticalLayout_->setSize(getWidth() - indentation_, verticalLayout_->getHeight());
            verticalLayout_->markContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            position.x = getWidth() - verticalLayout_->getWidth();
            break;
        case HorizontalAlignment::Left:
            break;
        }
        if (!expanded_) {
            // TODO: bad code
            position.x = -1000;
            position.y = -1000;
        }
        verticalLayout_->setPosition(position);
    }

    needToUpdateLayout_ = false;
}

void TreeView::doLayout()
{
    updateLayout();
}

void TreeView::draw(DrawingContext& drawingContext)
{
    updateLayout();

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    drawingContext.pushTransform(globalPos);

    horizontalLayout_->draw(drawingContext);

    if (expanded_) {
        verticalLayout_->draw(drawingContext);
    }

    drawingContext.popTransform();
}

} // namespace pomdog::gui
