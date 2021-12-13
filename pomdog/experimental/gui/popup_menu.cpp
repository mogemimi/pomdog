// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/popup_menu.hpp"
#include "pomdog/experimental/gui/context_menu.hpp"
#include "pomdog/experimental/gui/context_menu_item.hpp"
#include "pomdog/experimental/gui/drawing_context.hpp"
#include "pomdog/experimental/gui/pointer_point.hpp"
#include "pomdog/experimental/gui/ui_event_dispatcher.hpp"
#include "pomdog/experimental/gui/ui_helper.hpp"
#include "pomdog/math/math.hpp"

namespace pomdog::gui {

PopupMenu::PopupMenu(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , fontWeight(FontWeight::Normal)
    , textAlignment(TextAlignment::Left)
    , horizontalAlignment(HorizontalAlignment::Stretch)
    , verticalAlignment(VerticalAlignment::Top)
    , isEnabled(true)
    , isHovered(false)
    , isPressed(false)
    , isFocused(false)
{
    SetSize(74, 24);
}

bool PopupMenu::IsEnabled() const
{
    return isEnabled;
}

void PopupMenu::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
}

bool PopupMenu::IsHovered() const
{
    return isHovered;
}

bool PopupMenu::IsFocused() const
{
    return isFocused || (contextMenu != nullptr);
}

void PopupMenu::AddItem(const std::string& text)
{
    PopupMenuItem item;
    item.text = text;
    items.push_back(std::move(item));
}

void PopupMenu::ClearItems()
{
    items.clear();
}

std::string PopupMenu::GetItem(int index) const
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(items.size()));
    return items[index].text;
}

int PopupMenu::GetItemCount() const noexcept
{
    return static_cast<int>(items.size());
}

int PopupMenu::GetCurrentIndex() const noexcept
{
    return currentIndex;
}

void PopupMenu::SetCurrentIndex(int index)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(items.size()));
    currentIndex = index;
}

void PopupMenu::SetFontWeight(FontWeight fontWeightIn)
{
    this->fontWeight = fontWeightIn;
}

std::string PopupMenu::GetText() const
{
    POMDOG_ASSERT(currentIndex >= 0);
    POMDOG_ASSERT(currentIndex < static_cast<int>(items.size()));

    if (items.empty() || (currentIndex >= static_cast<int>(items.size()))) {
        return {};
    }
    return GetItem(currentIndex);
}

void PopupMenu::SetTextAlignment(TextAlignment textAlign)
{
    this->textAlignment = textAlign;
}

void PopupMenu::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    this->horizontalAlignment = horizontalAlignmentIn;
}

void PopupMenu::SetVerticalAlignment(VerticalAlignment verticalAlignmentIn) noexcept
{
    this->verticalAlignment = verticalAlignmentIn;
}

HorizontalAlignment PopupMenu::GetHorizontalAlignment() const noexcept
{
    return horizontalAlignment;
}

VerticalAlignment PopupMenu::GetVerticalAlignment() const noexcept
{
    return verticalAlignment;
}

void PopupMenu::OnEnter()
{
}

void PopupMenu::OnFocusIn()
{
    isFocused = true;
}

void PopupMenu::OnFocusOut()
{
    isFocused = false;
}

void PopupMenu::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isHovered = true;
}

void PopupMenu::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isHovered = false;
}

void PopupMenu::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPressed = true;
}

void PopupMenu::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isPressed = false;

    if (contextMenu == nullptr) {
        auto dispatcher = GetDispatcher();
        contextMenu = std::make_shared<ContextMenu>(dispatcher, GetWidth(), GetHeight());

        std::shared_ptr<ContextMenuButton> focusedItem;
        int totalHeight = 0;

        for (int index = 0; index < static_cast<int>(items.size()); index++) {
            const auto& item = items[index];

            auto button = std::make_shared<ContextMenuButton>(dispatcher);
            button->SetText(item.text);
            button->SetMargin(Thickness{0, 8, 0, 8});
            contextMenu->AddChild(button);

            connections(button->Click, [this, index] {
                const bool changed = (this->currentIndex != index);
                this->currentIndex = index;

                auto dispatcherLocal = GetDispatcher();
                dispatcherLocal->SetFocusWidget(shared_from_this());
                dispatcherLocal->RemoveContextMenu(contextMenu);
                contextMenu = nullptr;

                if (changed) {
                    this->CurrentIndexChanged(index);
                }
            });

            if (currentIndex == index) {
                focusedItem = button;
            }

            totalHeight += button->GetHeight();
        }

        constexpr int maxScrollViewHeight = 120;
        contextMenu->SetSize(contextMenu->GetWidth(), std::min(maxScrollViewHeight, totalHeight + 2));

        dispatcher->AddContextMenu(contextMenu);
        contextMenu->SetPadding(Thickness{1, 0, 1, 0});
        contextMenu->DoLayout();
        contextMenu->SetPosition(GetGlobalPosition() - Point2D{0, contextMenu->GetHeight()});

        if (focusedItem != nullptr) {
            contextMenu->ScrollTo(focusedItem);
        }

        focusConn = dispatcher->FocusChanged.Connect([this](const std::shared_ptr<Widget>& focusedWidget) {
            auto widget = focusedWidget;

            const auto hasFocus = [&]() -> bool {
                if (widget == shared_from_this()) {
                    return true;
                }
                while (widget != nullptr) {
                    if (widget == contextMenu) {
                        return true;
                    }
                    widget = widget->GetParent();
                }
                return false;
            }();

            if (!hasFocus) {
                if (contextMenu != nullptr) {
                    auto dispatcherLocal = GetDispatcher();
                    dispatcherLocal->RemoveContextMenu(contextMenu);
                    contextMenu = nullptr;
                }
                focusConn.Disconnect();
            }
        });

        if (focusedItem) {
            dispatcher->SetFocusWidget(focusedItem);
        }
        else {
            dispatcher->SetFocusWidget(contextMenu);
        }
    }
    else {
        auto dispatcher = GetDispatcher();
        dispatcher->RemoveContextMenu(contextMenu);
        contextMenu = nullptr;
    }
}

void PopupMenu::Draw(DrawingContext& drawingContext)
{
    const Color arrowTextColor = Color{192, 189, 190, 255};
    const Color textNormalColor = {251, 250, 248, 255};
    const Color textDisabledColor = {191, 190, 189, 255};

    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    auto textColor = textNormalColor;

    if (!isEnabled) {
        textColor = textDisabledColor;
    }
    else if (isPressed) {
        textColor = textNormalColor;
    }
    else if (isHovered) {
        textColor = textNormalColor;
    }

    auto fieldColor = colorScheme->TextFieldRectColorBase;
    auto borderColor = colorScheme->TextFieldBorderColorBase;

    if (this->IsFocused()) {
        borderColor = colorScheme->TextFieldBorderColorFocus;
    }

    // NOTE: Draw box border
    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X, globalPos.Y, GetWidth(), GetHeight()},
        borderColor);

    // NOTE: Draw text field box
    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X + 1, globalPos.Y + 1, GetWidth() - 2, GetHeight() - 2},
        fieldColor);

    primitiveBatch->Flush();

    auto text = GetText();

    if (!text.empty()) {
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto spriteFont = drawingContext.GetFont(fontWeight, FontSize::Medium);

        const auto buttonPos = math::ToVector2(globalPos);
        const auto buttonSize = Vector2{static_cast<float>(GetWidth()), static_cast<float>(GetHeight())};

        const auto baselineHeight = 3.0f;
        const auto horizontalPadding = 6.0f;

        auto originPivot = Vector2::Zero();
        Vector2 padding = Vector2{0.0f, baselineHeight};
        switch (textAlignment) {
        case TextAlignment::Left:
            originPivot = Vector2{0.0f, 0.5f};
            padding.X = horizontalPadding;
            break;
        case TextAlignment::Center:
            originPivot = Vector2{0.5f, 0.5f};
            break;
        case TextAlignment::Right:
            originPivot = Vector2{1.0f, 0.5f};
            padding.X = -horizontalPadding;
            break;
        }

        const auto textPosition = buttonPos + buttonSize * originPivot + padding;

        spriteFont->Draw(
            *spriteBatch,
            text,
            textPosition,
            textColor,
            0.0f,
            originPivot,
            1.0f);

        spriteBatch->Flush();
    }

    constexpr auto iconBaseSize = 32.0f;
    constexpr auto iconScaleSize = 12.0f;
    constexpr auto paddingRight = 2.0f;
    auto transformOffset = math::ToVector2(globalPos)
        + Vector2{static_cast<float>(GetWidth()), static_cast<float>(GetHeight()) * 0.5f}
        - Vector2{iconScaleSize + paddingRight, iconScaleSize * 0.5f + 1.0f};

    drawingContext.DrawIcon(
        "ionicons/ios-arrow-down.svg",
        transformOffset,
        arrowTextColor,
        0.0f,
        Vector2::Zero(),
        iconScaleSize / iconBaseSize);
}

} // namespace pomdog::gui
