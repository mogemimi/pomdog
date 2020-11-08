// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/NumberField.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/PushButton.hpp"
#include "Pomdog/Experimental/GUI/TextBlock.hpp"
#include "Pomdog/Experimental/GUI/TextEdit.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Math/Math.hpp"
#include <array>

namespace Pomdog::GUI {
namespace {

constexpr int prependAreaWidth = 10;

} // namespace

NumberField::NumberField(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    const std::shared_ptr<NumberFieldDataContext>& dataContextIn)
    : Widget(dispatcher)
    , dataContext(dataContextIn)
    , prependAreaColor(255, 255, 255, 0)
    , isPointerEntered(false)
    , isDragging(false)
{
    SetSize(50, 18);
    SetCursor(MouseCursor::ResizeHorizontal);

    textEdit = std::make_shared<TextEdit>(dispatcher);
    textEdit->SetTextMargin(Thickness{1, 4, 1, 4});
    textEdit->SetBaselineHeight(4.0f);
    textEdit->SetText(dataContext->ToString());

    textBlock = std::make_shared<TextBlock>(dispatcher);
    textBlock->SetBaselineHeight(4.0f);
    textBlock->SetTextAlignment(TextAlignment::Center);
    textBlock->SetColor(Color::White);
    textBlock->SetText(dataContext->ToString());

    addButton = std::make_shared<PushButton>(dispatcher);
    addButton->SetSize(10, GetHeight());
    addButton->SetText(">");

    subButton = std::make_shared<PushButton>(dispatcher);
    subButton->SetSize(10, GetHeight());
    subButton->SetText("<");
}

void NumberField::SetFontWeight(FontWeight fontWeightIn)
{
    textEdit->SetFontWeight(fontWeightIn);
}

void NumberField::SetFontSize(FontSize fontSizeIn)
{
    textEdit->SetFontSize(fontSizeIn);
}

bool NumberField::IsEnabled() const
{
    return textEdit->IsEnabled();
}

void NumberField::SetEnabled(bool enabledIn)
{
    textEdit->SetEnabled(enabledIn);
}

bool NumberField::IsReadOnly() const
{
    return textEdit->IsReadOnly();
}

void NumberField::SetReadOnly(bool readOnlyIn)
{
    textEdit->SetReadOnly(readOnlyIn);
}

bool NumberField::IsAcceptable() const
{
    return textEdit->IsAcceptable();
}

void NumberField::SetAcceptable(bool acceptableIn)
{
    textEdit->SetAcceptable(acceptableIn);
}

void NumberField::SetText(const std::string& text)
{
    textEdit->SetText(text);
    textBlock->SetText(text);
}

std::string NumberField::GetPlaceholderText() const
{
    return textEdit->GetPlaceholderText();
}

void NumberField::SetPlaceholderText(const std::string& textIn)
{
    textEdit->SetPlaceholderText(textIn);
}

bool NumberField::HasSelectedText() const
{
    return textEdit->HasSelectedText();
}

void NumberField::Deselect()
{
    textEdit->Deselect();
}

void NumberField::SetPrependAreaColor(const std::optional<Color>& color)
{
    if (color != std::nullopt) {
        prependAreaColor = Color{color->R, color->G, color->B, 255};
    }
    else {
        prependAreaColor = Color{255, 255, 255, 0};
    }
}

void NumberField::SetPrependAreaString(const std::string& text)
{
    prependAreaText = text;
}

void NumberField::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    textEdit->SetHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment NumberField::GetHorizontalAlignment() const noexcept
{
    return textEdit->GetHorizontalAlignment();
}

VerticalAlignment NumberField::GetVerticalAlignment() const noexcept
{
    return textEdit->GetVerticalAlignment();
}

void NumberField::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    POMDOG_ASSERT(textEdit != nullptr);
    POMDOG_ASSERT(textBlock != nullptr);
    POMDOG_ASSERT(addButton != nullptr);
    POMDOG_ASSERT(subButton != nullptr);
    textEdit->MarkParentTransformDirty();
    textBlock->MarkParentTransformDirty();
    addButton->MarkParentTransformDirty();
    subButton->MarkParentTransformDirty();
}

void NumberField::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();

    POMDOG_ASSERT(textEdit != nullptr);
    POMDOG_ASSERT(textBlock != nullptr);
    POMDOG_ASSERT(addButton != nullptr);
    POMDOG_ASSERT(subButton != nullptr);
    textEdit->MarkParentTransformDirty();
    textBlock->MarkParentTransformDirty();
    addButton->MarkParentTransformDirty();
    subButton->MarkParentTransformDirty();
}

void NumberField::OnEnter()
{
    std::array<std::shared_ptr<Widget>, 4> children = {{
        textEdit,
        textBlock,
        addButton,
        subButton,
    }};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->OnEnter();
        child->MarkParentTransformDirty();
        child->SetParent(shared_from_this());
    }

    textSubmittedConn = textEdit->TextSubmitted.Connect([this] {
        auto text = textEdit->GetText();
        if (!dataContext->TextSubmitted(text)) {
            textEdit->SetText(dataContext->ToString());
            return;
        }

        textEdit->SetText(dataContext->ToString());
        textBlock->SetText(textEdit->GetText());

        this->ValueChanged();
    });

    focusOutConn = textEdit->FocusOut.Connect([this] {
        textEdit->Deselect();
        textEdit->SetVisible(false);
        textEdit->SetText(dataContext->ToString());
        textBlock->SetText(textEdit->GetText());
    });

    addButtonConn = addButton->Click.Connect([this] {
        dataContext->IncrementValue();
        textBlock->SetText(dataContext->ToString());
        ValueChanged();
    });

    subButtonConn = subButton->Click.Connect([this] {
        dataContext->DecrementValue();
        textBlock->SetText(dataContext->ToString());
        ValueChanged();
    });

    textEdit->SetVisible(false);
    addButton->SetVisible(false);
    subButton->SetVisible(false);
}

void NumberField::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPointerEntered = true;

    if (!textEdit->IsFocused()) {
        addButton->SetVisible(true);
        subButton->SetVisible(true);
    }
}

void NumberField::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPointerEntered = false;
}

void NumberField::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!textEdit->IsEnabled()) {
        return;
    }

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
    startDragPosition = pointInView;
    dataContext->BeginDragging();
}

void NumberField::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!textEdit->IsEnabled()) {
        return;
    }

    if (startDragPosition == std::nullopt) {
        return;
    }

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
    auto amount = (pointInView.X - startDragPosition->X);

    if (pointInView != startDragPosition) {
        isDragging = true;
    }

    if (isDragging) {
        dataContext->UpdateDragging(amount);
        textBlock->SetText(dataContext->ToString());
        ValueChanged();
    }
}

void NumberField::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!textEdit->IsEnabled()) {
        return;
    }

    if (!isDragging) {
        textEdit->SetVisible(true);

        auto dispatcher = GetDispatcher();
        dispatcher->SetFocusWidget(textEdit);

        // NOTE: Remove trailing zeros from string
        auto s = textBlock->GetText();
        textEdit->SetText(dataContext->ToEditableString(s));

        textEdit->SetSelection(0, static_cast<int>(textEdit->GetText().size()));

        addButton->SetVisible(false);
        subButton->SetVisible(false);
    }

    isDragging = false;
    startDragPosition = std::nullopt;
}

std::shared_ptr<Widget> NumberField::GetChildAt(const Point2D& position)
{
    std::array<std::shared_ptr<Widget>, 3> children = {{addButton, subButton, textEdit}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);

        if (!child->IsVisible()) {
            continue;
        }

        auto bounds = child->GetBounds();
        if (bounds.Contains(position)) {
            return child;
        }
    }
    return nullptr;
}

void NumberField::UpdateAnimation(const Duration& frameDuration)
{
    std::array<std::shared_ptr<Widget>, 3> children = {{addButton, subButton, textEdit}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->UpdateAnimation(frameDuration);
    }
}

void NumberField::DoLayout()
{
    int prependAreaOffset = 0;
    const bool hasPrependArea = (prependAreaColor.A == 255);
    if (hasPrependArea) {
        prependAreaOffset = prependAreaWidth;
    }

    POMDOG_ASSERT(textEdit != nullptr);
    textEdit->SetSize(GetWidth() - prependAreaOffset, GetHeight());
    textEdit->SetPosition(Point2D{prependAreaOffset, 0});
    textEdit->DoLayout();

    POMDOG_ASSERT(textBlock != nullptr);
    auto margin = textEdit->GetTextMargin();
    textBlock->SetSize(GetWidth() - prependAreaOffset, GetHeight());
    textBlock->SetPosition(Point2D{prependAreaOffset, margin.Bottom});
    textBlock->DoLayout();

    POMDOG_ASSERT(addButton != nullptr);
    addButton->SetPosition(Point2D{GetWidth() - addButton->GetWidth(), 0});
    addButton->DoLayout();

    POMDOG_ASSERT(subButton != nullptr);
    subButton->SetPosition(Point2D{prependAreaOffset, 0});
    subButton->DoLayout();
}

void NumberField::Draw(DrawingContext& drawingContext)
{
    if (!isPointerEntered && !addButton->IsHovered() && !subButton->IsHovered()) {
        addButton->SetVisible(false);
        subButton->SetVisible(false);
    }

    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    auto fieldColor = colorScheme->TextFieldRectColorBase;
    auto borderColor = colorScheme->TextFieldBorderColorBase;

    if (!textEdit->IsAcceptable()) {
        borderColor = colorScheme->TextFieldBorderColorError;
    }
    else if (textEdit->IsFocused()) {
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

    const bool hasPrependArea = (prependAreaColor.A == 255);
    if (hasPrependArea) {
        // NOTE: Draw prepend area box
        primitiveBatch->DrawRectangle(
            Rectangle{globalPos.X + 1, globalPos.Y + 1, prependAreaWidth, GetHeight() - 2},
            prependAreaColor);
    }

    primitiveBatch->Flush();

    if (hasPrependArea && !prependAreaText.empty()) {
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto spriteFont = drawingContext.GetFont(FontWeight::Bold, FontSize::Small);

        spriteFont->Draw(
            *spriteBatch,
            prependAreaText,
            Math::ToVector2(globalPos + Point2D{prependAreaWidth / 2, GetHeight() / 2}) + Vector2{1.0f, 2.5f},
            Color{231, 229, 230, 255},
            0.0f,
            Vector2{0.5f, 0.5f},
            0.9f);

        spriteBatch->Flush();
    }

    drawingContext.PushTransform(globalPos);

    POMDOG_ASSERT(textEdit != nullptr);
    if (textEdit->IsVisible()) {
        textEdit->Draw(drawingContext);
    }
    else {
        POMDOG_ASSERT(textBlock != nullptr);
        textBlock->Draw(drawingContext);

        auto spriteBatch = drawingContext.GetSpriteBatch();
        spriteBatch->Flush();
    }

    POMDOG_ASSERT(addButton != nullptr);
    if (addButton->IsVisible()) {
        addButton->Draw(drawingContext);
    }

    POMDOG_ASSERT(subButton != nullptr);
    if (subButton->IsVisible()) {
        subButton->Draw(drawingContext);
    }

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI
