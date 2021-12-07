// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/int_field.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/drawing_context.hpp"
#include "pomdog/experimental/gui/pointer_point.hpp"
#include "pomdog/experimental/gui/push_button.hpp"
#include "pomdog/experimental/gui/text_block.hpp"
#include "pomdog/experimental/gui/text_edit.hpp"
#include "pomdog/experimental/gui/ui_event_dispatcher.hpp"
#include "pomdog/experimental/gui/ui_helper.hpp"
#include "pomdog/utility/string_helper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI::Detail {

std::string IntFieldDataContext::ToString() const
{
    return std::to_string(value);
}

std::string IntFieldDataContext::ToEditableString(const std::string& text) const
{
    return text;
}

int IntFieldDataContext::GetValue() const noexcept
{
    return value;
}

void IntFieldDataContext::SetValue(int valueIn)
{
    value = valueIn;
}

void IntFieldDataContext::IncrementValue()
{
    constexpr int unit = 1;
    value = value + unit;
}

void IntFieldDataContext::DecrementValue()
{
    constexpr int unit = 1;
    value = value - unit;
}

void IntFieldDataContext::BeginDragging()
{
    startDragValue = value;
}

void IntFieldDataContext::UpdateDragging(int amount)
{
    constexpr int unit = 1;
    value = startDragValue + amount * unit;
}

bool IntFieldDataContext::TextSubmitted(const std::string& text)
{
    std::optional<int> newValue;
    try {
        newValue = std::stoi(text);
    }
    catch (const std::invalid_argument&) {
        newValue = std::nullopt;
    }
    catch (const std::out_of_range&) {
        newValue = std::nullopt;
    }

    if (!newValue) {
        return false;
    }

    value = *newValue;
    return true;
}

} // namespace Pomdog::GUI::Detail

namespace Pomdog::GUI {

IntField::IntField(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
{
    dataContext = std::make_shared<Detail::IntFieldDataContext>();
    numberField = std::make_shared<NumberField>(dispatcher, dataContext);
    SetSize(numberField->GetWidth(), numberField->GetHeight());
}

void IntField::SetFontWeight(FontWeight fontWeightIn)
{
    numberField->SetFontWeight(fontWeightIn);
}

void IntField::SetFontSize(FontSize fontSizeIn)
{
    numberField->SetFontSize(fontSizeIn);
}

bool IntField::IsEnabled() const
{
    return numberField->IsEnabled();
}

void IntField::SetEnabled(bool enabledIn)
{
    numberField->SetEnabled(enabledIn);
}

bool IntField::IsReadOnly() const
{
    return numberField->IsReadOnly();
}

void IntField::SetReadOnly(bool readOnlyIn)
{
    numberField->SetReadOnly(readOnlyIn);
}

bool IntField::IsAcceptable() const
{
    return numberField->IsAcceptable();
}

void IntField::SetAcceptable(bool acceptableIn)
{
    numberField->SetAcceptable(acceptableIn);
}

int IntField::GetValue() const
{
    POMDOG_ASSERT(dataContext != nullptr);
    return dataContext->GetValue();
}

void IntField::SetValue(int valueIn)
{
    POMDOG_ASSERT(dataContext != nullptr);
    dataContext->SetValue(valueIn);

    POMDOG_ASSERT(numberField != nullptr);
    numberField->SetText(dataContext->ToString());
}

std::string IntField::GetPlaceholderText() const
{
    return numberField->GetPlaceholderText();
}

void IntField::SetPlaceholderText(const std::string& textIn)
{
    numberField->SetPlaceholderText(textIn);
}

bool IntField::HasSelectedText() const
{
    return numberField->HasSelectedText();
}

void IntField::Deselect()
{
    numberField->Deselect();
}

void IntField::SetPrependAreaColor(const std::optional<Color>& color)
{
    numberField->SetPrependAreaColor(color);
}

void IntField::SetPrependAreaString(const std::string& text)
{
    numberField->SetPrependAreaString(text);
}

void IntField::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    numberField->SetHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment IntField::GetHorizontalAlignment() const noexcept
{
    return numberField->GetHorizontalAlignment();
}

VerticalAlignment IntField::GetVerticalAlignment() const noexcept
{
    return numberField->GetVerticalAlignment();
}

void IntField::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    POMDOG_ASSERT(numberField != nullptr);
    numberField->MarkParentTransformDirty();
}

void IntField::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();

    POMDOG_ASSERT(numberField != nullptr);
    numberField->MarkParentTransformDirty();
}

void IntField::OnEnter()
{
    if (numberField != nullptr) {
        numberField->OnEnter();
        numberField->MarkParentTransformDirty();
        numberField->SetParent(shared_from_this());

        valueChangedConn = numberField->ValueChanged.Connect([this] {
            this->ValueChanged(dataContext->GetValue());
        });
    }
}

std::shared_ptr<Widget> IntField::GetChildAt(const Point2D& position)
{
    if (numberField != nullptr) {
        auto bounds = numberField->GetBounds();
        if (bounds.Contains(position)) {
            return numberField;
        }
    }
    return nullptr;
}

void IntField::UpdateAnimation(const Duration& frameDuration)
{
    if (numberField != nullptr) {
        numberField->UpdateAnimation(frameDuration);
    }
}

void IntField::DoLayout()
{
    if (numberField != nullptr) {
        numberField->SetSize(GetWidth(), GetHeight());
        numberField->DoLayout();
    }
}

void IntField::Draw(DrawingContext& drawingContext)
{
    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    drawingContext.PushTransform(globalPos);

    if (numberField != nullptr) {
        numberField->Draw(drawingContext);
    }

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI
