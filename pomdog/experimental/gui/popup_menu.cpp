// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/popup_menu.h"
#include "pomdog/experimental/gui/context_menu.h"
#include "pomdog/experimental/gui/context_menu_item.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

PopupMenu::PopupMenu(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , fontWeight_(FontWeight::Normal)
    , textAlignment_(TextAlignment::Left)
    , horizontalAlignment_(HorizontalAlignment::Stretch)
    , verticalAlignment_(VerticalAlignment::Top)
    , isEnabled_(true)
    , isHovered_(false)
    , isPressed_(false)
    , isFocused_(false)
{
    setSize(74, 24);
}

bool PopupMenu::isEnabled() const
{
    return isEnabled_;
}

void PopupMenu::setEnabled(bool isEnabledIn)
{
    isEnabled_ = isEnabledIn;
}

bool PopupMenu::isHovered() const
{
    return isHovered_;
}

bool PopupMenu::isFocused() const
{
    return isFocused_ || (contextMenu_ != nullptr);
}

void PopupMenu::addItem(const std::string& text)
{
    PopupMenuItem item;
    item.text = text;
    items_.push_back(std::move(item));
}

void PopupMenu::clearItems()
{
    items_.clear();
}

std::string PopupMenu::getItem(int index) const
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(items_.size()));
    return items_[index].text;
}

int PopupMenu::getItemCount() const noexcept
{
    return static_cast<int>(items_.size());
}

int PopupMenu::getCurrentIndex() const noexcept
{
    return currentIndex_;
}

void PopupMenu::setCurrentIndex(int index)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(items_.size()));
    currentIndex_ = index;
}

void PopupMenu::setFontWeight(FontWeight fontWeightIn)
{
    fontWeight_ = fontWeightIn;
}

std::string PopupMenu::getText() const
{
    POMDOG_ASSERT(currentIndex_ >= 0);
    POMDOG_ASSERT(currentIndex_ < static_cast<int>(items_.size()));

    if (items_.empty() || (currentIndex_ >= static_cast<int>(items_.size()))) {
        return {};
    }
    return getItem(currentIndex_);
}

void PopupMenu::setTextAlignment(TextAlignment textAlign)
{
    textAlignment_ = textAlign;
}

void PopupMenu::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    horizontalAlignment_ = horizontalAlignmentIn;
}

void PopupMenu::setVerticalAlignment(VerticalAlignment verticalAlignmentIn) noexcept
{
    verticalAlignment_ = verticalAlignmentIn;
}

HorizontalAlignment PopupMenu::getHorizontalAlignment() const noexcept
{
    return horizontalAlignment_;
}

VerticalAlignment PopupMenu::getVerticalAlignment() const noexcept
{
    return verticalAlignment_;
}

void PopupMenu::onEnter()
{
}

void PopupMenu::onFocusIn()
{
    isFocused_ = true;
}

void PopupMenu::onFocusOut()
{
    isFocused_ = false;
}

void PopupMenu::onPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isHovered_ = true;
}

void PopupMenu::onPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isHovered_ = false;
}

void PopupMenu::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPressed_ = true;
}

void PopupMenu::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isPressed_ = false;

    if (contextMenu_ == nullptr) {
        auto dispatcher = getDispatcher();
        contextMenu_ = std::make_shared<ContextMenu>(dispatcher, getWidth(), getHeight());

        std::shared_ptr<ContextMenuButton> focusedItem;
        int totalHeight = 0;

        for (int index = 0; index < static_cast<int>(items_.size()); index++) {
            const auto& item = items_[index];

            auto button = std::make_shared<ContextMenuButton>(dispatcher);
            button->setText(item.text);
            button->setMargin(Thickness{0, 8, 0, 8});
            contextMenu_->addChild(button);

            connections_(button->Click, [this, index] {
                const bool changed = (currentIndex_ != index);
                currentIndex_ = index;

                auto dispatcherLocal = getDispatcher();
                dispatcherLocal->setFocusWidget(shared_from_this());
                dispatcherLocal->RemoveContextMenu(contextMenu_);
                contextMenu_ = nullptr;

                if (changed) {
                    this->CurrentIndexChanged(index);
                }
            });

            if (currentIndex_ == index) {
                focusedItem = button;
            }

            totalHeight += button->getHeight();
        }

        constexpr int maxScrollViewHeight = 120;
        contextMenu_->setSize(contextMenu_->getWidth(), std::min(maxScrollViewHeight, totalHeight + 2));

        dispatcher->AddContextMenu(contextMenu_);
        contextMenu_->setPadding(Thickness{1, 0, 1, 0});
        contextMenu_->doLayout();
        contextMenu_->setPosition(getGlobalPosition() - Point2D{0, contextMenu_->getHeight()});

        if (focusedItem != nullptr) {
            contextMenu_->scrollTo(focusedItem);
        }

        focusConn_ = dispatcher->FocusChanged.connect([this](const std::shared_ptr<Widget>& focusedWidget) {
            auto widget = focusedWidget;

            const auto hasFocus = [&]() -> bool {
                if (widget == shared_from_this()) {
                    return true;
                }
                while (widget != nullptr) {
                    if (widget == contextMenu_) {
                        return true;
                    }
                    widget = widget->getParent();
                }
                return false;
            }();

            if (!hasFocus) {
                if (contextMenu_ != nullptr) {
                    auto dispatcherLocal = getDispatcher();
                    dispatcherLocal->RemoveContextMenu(contextMenu_);
                    contextMenu_ = nullptr;
                }
                focusConn_.disconnect();
            }
        });

        if (focusedItem) {
            dispatcher->setFocusWidget(focusedItem);
        }
        else {
            dispatcher->setFocusWidget(contextMenu_);
        }
    }
    else {
        auto dispatcher = getDispatcher();
        dispatcher->RemoveContextMenu(contextMenu_);
        contextMenu_ = nullptr;
    }
}

void PopupMenu::draw(DrawingContext& drawingContext)
{
    const Color arrowTextColor = Color{192, 189, 190, 255};
    const Color textNormalColor = {251, 250, 248, 255};
    const Color textDisabledColor = {191, 190, 189, 255};

    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    auto textColor = textNormalColor;

    if (!isEnabled_) {
        textColor = textDisabledColor;
    }
    else if (isPressed_) {
        textColor = textNormalColor;
    }
    else if (isHovered_) {
        textColor = textNormalColor;
    }

    auto fieldColor = colorScheme->TextFieldRectColorBase;
    auto borderColor = colorScheme->TextFieldBorderColorBase;

    if (this->isFocused()) {
        borderColor = colorScheme->TextFieldBorderColorFocus;
    }

    // NOTE: draw box border
    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
        borderColor);

    // NOTE: draw text field box
    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x + 1, globalPos.y + 1, getWidth() - 2, getHeight() - 2},
        fieldColor);

    primitiveBatch->flush();

    auto text = getText();

    if (!text.empty()) {
        auto spriteBatch = drawingContext.getSpriteBatch();
        auto spriteFont = drawingContext.getFont(fontWeight_, FontSize::Medium);

        const auto buttonPos = math::toVector2(globalPos);
        const auto buttonSize = Vector2{static_cast<float>(getWidth()), static_cast<float>(getHeight())};

        const auto baselineHeight = 3.0f;
        const auto horizontalPadding = 6.0f;

        auto originPivot = Vector2::createZero();
        Vector2 padding = Vector2{0.0f, baselineHeight};
        switch (textAlignment_) {
        case TextAlignment::Left:
            originPivot = Vector2{0.0f, 0.5f};
            padding.x = horizontalPadding;
            break;
        case TextAlignment::Center:
            originPivot = Vector2{0.5f, 0.5f};
            break;
        case TextAlignment::Right:
            originPivot = Vector2{1.0f, 0.5f};
            padding.x = -horizontalPadding;
            break;
        }

        const auto textPosition = buttonPos + buttonSize * originPivot + padding;

        spriteFont->draw(
            *spriteBatch,
            text,
            textPosition,
            textColor,
            0.0f,
            originPivot,
            1.0f);

        drawingContext.flushSpriteBatch();
    }

    constexpr auto iconBaseSize = 32.0f;
    constexpr auto iconScaleSize = 12.0f;
    constexpr auto paddingRight = 2.0f;
    auto transformOffset =
        math::toVector2(globalPos) +
        Vector2{static_cast<float>(getWidth()), static_cast<float>(getHeight()) * 0.5f} -
        Vector2{iconScaleSize + paddingRight, iconScaleSize * 0.5f + 1.0f};

    drawingContext.drawIcon(
        "ios-arrow-down.svg",
        transformOffset,
        arrowTextColor,
        0.0f,
        Vector2::createZero(),
        iconScaleSize / iconBaseSize);
}

} // namespace pomdog::gui
