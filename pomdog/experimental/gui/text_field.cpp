// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/text_field.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/text_edit.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"

namespace pomdog::gui {

TextField::TextField(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
{
    SetSize(50, 22);

    textEdit = std::make_shared<TextEdit>(dispatcher);
    textEdit->SetSize(GetWidth(), GetHeight());
    textEdit->SetBaselineHeight(3.0f);
}

void TextField::SetFontWeight(FontWeight fontWeightIn)
{
    textEdit->SetFontWeight(fontWeightIn);
}

void TextField::SetFontSize(FontSize fontSizeIn)
{
    textEdit->SetFontSize(fontSizeIn);
}

bool TextField::IsEnabled() const
{
    return textEdit->IsEnabled();
}

void TextField::SetEnabled(bool enabledIn)
{
    textEdit->SetEnabled(enabledIn);
}

bool TextField::IsReadOnly() const
{
    return textEdit->IsReadOnly();
}

void TextField::SetReadOnly(bool readOnlyIn)
{
    textEdit->SetReadOnly(readOnlyIn);
}

bool TextField::IsAcceptable() const
{
    return textEdit->IsAcceptable();
}

void TextField::SetAcceptable(bool acceptableIn)
{
    textEdit->SetAcceptable(acceptableIn);
}

std::string TextField::GetText() const
{
    return textEdit->GetText();
}

void TextField::SetText(const std::string& textIn)
{
    textEdit->SetText(textIn);
}

std::string TextField::GetPlaceholderText() const
{
    return textEdit->GetPlaceholderText();
}

void TextField::SetPlaceholderText(const std::string& textIn)
{
    textEdit->SetPlaceholderText(textIn);
}

bool TextField::HasSelectedText() const
{
    return textEdit->HasSelectedText();
}

void TextField::Deselect()
{
    textEdit->Deselect();
}

void TextField::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    textEdit->SetHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment TextField::GetHorizontalAlignment() const noexcept
{
    return textEdit->GetHorizontalAlignment();
}

VerticalAlignment TextField::GetVerticalAlignment() const noexcept
{
    return textEdit->GetVerticalAlignment();
}

void TextField::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);
    POMDOG_ASSERT(textEdit != nullptr);
    textEdit->MarkParentTransformDirty();
}

void TextField::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();

    POMDOG_ASSERT(textEdit != nullptr);
    textEdit->MarkParentTransformDirty();
}

std::shared_ptr<Widget> TextField::GetChildAt(const Point2D& position)
{
    if (textEdit != nullptr) {
        const auto bounds = textEdit->GetBounds();
        if (bounds.contains(position)) {
            return textEdit;
        }
    }
    return nullptr;
}

void TextField::UpdateAnimation(const Duration& frameDuration)
{
    if (textEdit != nullptr) {
        textEdit->UpdateAnimation(frameDuration);
    }
}

void TextField::OnEnter()
{
    POMDOG_ASSERT(textEdit != nullptr);
    textEdit->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    textEdit->SetParent(shared_from_this());
    textEdit->OnEnter();

    textChangedConn = textEdit->TextChanged.connect([this] { this->TextChanged(); });
}

void TextField::DoLayout()
{
    POMDOG_ASSERT(textEdit != nullptr);
    textEdit->SetSize(GetWidth(), GetHeight());
    textEdit->DoLayout();
}

void TextField::Draw(DrawingContext& drawingContext)
{
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
    primitiveBatch->drawRectangle(
        Rectangle{globalPos.x, globalPos.y, GetWidth(), GetHeight()},
        borderColor);

    // NOTE: Draw text field box
    primitiveBatch->drawRectangle(
        Rectangle{globalPos.x + 1, globalPos.y + 1, GetWidth() - 2, GetHeight() - 2},
        fieldColor);

    primitiveBatch->flush();

    drawingContext.PushTransform(globalPos);

    POMDOG_ASSERT(textEdit != nullptr);
    textEdit->Draw(drawingContext);

    drawingContext.PopTransform();
}

} // namespace pomdog::gui
