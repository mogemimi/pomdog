// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/float_field.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/push_button.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/text_edit.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui::detail {

std::string FloatFieldDataContext::ToString() const
{
    switch (precision) {
    case 0:
        return strings::format("%.1lf", value);
    case 1:
        return strings::format("%.1lf", value);
    case 2:
        return strings::format("%.2lf", value);
    case 3:
        return strings::format("%.3lf", value);
    case 4:
        return strings::format("%.4lf", value);
    default:
        break;
    }
    return strings::format("%lf", value);
}

std::string FloatFieldDataContext::ToEditableString(const std::string& text) const
{
    // NOTE: Remove trailing zeros from string.
    auto v = strings::trimRight(text, '0');
    if (strings::hasSuffix(v, ".")) {
        v = std::string_view{v.data(), std::min(v.size() + 1, text.size())};
    }
    return std::string{v};
}

double FloatFieldDataContext::GetValue() const noexcept
{
    return value;
}

void FloatFieldDataContext::SetValue(double valueIn)
{
    value = valueIn;
}

double FloatFieldDataContext::GetMinimum() const noexcept
{
    return minimum;
}

void FloatFieldDataContext::SetMinimum(double minimumIn)
{
    minimum = minimumIn;
}

double FloatFieldDataContext::GetMaximum() const noexcept
{
    return maximum;
}

void FloatFieldDataContext::SetMaximum(double maximumIn)
{
    maximum = maximumIn;
}

int FloatFieldDataContext::GetDecimals() const noexcept
{
    return precision;
}

void FloatFieldDataContext::SetDecimals(int precisionIn)
{
    precision = precisionIn;
}

double FloatFieldDataContext::GetUnit() const noexcept
{
    if ((0 <= precision) && (precision <= 9)) {
        const std::array<double, 10> units = {{
            1.0,
            0.1,
            0.01,
            0.001,
            0.0001,
            0.00001,
            0.000001,
            0.0000001,
            0.00000001,
            0.000000001,
        }};
        return units[precision];
    }
    return 0.01;
}

void FloatFieldDataContext::IncrementValue()
{
    const double unit = GetUnit();
    value = std::clamp(value + unit, minimum, maximum);
}

void FloatFieldDataContext::DecrementValue()
{
    const double unit = GetUnit();
    value = std::clamp(value - unit, minimum, maximum);
}

void FloatFieldDataContext::BeginDragging()
{
    startDragValue = value;
}

void FloatFieldDataContext::UpdateDragging(int amount)
{
    const double unit = GetUnit();
    value = std::clamp(startDragValue + amount * unit, minimum, maximum);
}

bool FloatFieldDataContext::TextSubmitted(const std::string& text)
{
    std::optional<double> newValue;
    try {
        newValue = std::stod(text);
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

    value = std::clamp(*newValue, minimum, maximum);
    return true;
}

} // namespace pomdog::gui::detail

namespace pomdog::gui {

FloatField::FloatField(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
{
    dataContext = std::make_shared<detail::FloatFieldDataContext>();
    numberField = std::make_shared<NumberField>(dispatcher, dataContext);
    SetSize(numberField->GetWidth(), numberField->GetHeight());
}

void FloatField::SetFontWeight(FontWeight fontWeightIn)
{
    numberField->SetFontWeight(fontWeightIn);
}

void FloatField::SetFontSize(FontSize fontSizeIn)
{
    numberField->SetFontSize(fontSizeIn);
}

bool FloatField::IsEnabled() const
{
    return numberField->IsEnabled();
}

void FloatField::SetEnabled(bool enabledIn)
{
    numberField->SetEnabled(enabledIn);
}

bool FloatField::IsReadOnly() const
{
    return numberField->IsReadOnly();
}

void FloatField::SetReadOnly(bool readOnlyIn)
{
    numberField->SetReadOnly(readOnlyIn);
}

bool FloatField::IsAcceptable() const
{
    return numberField->IsAcceptable();
}

void FloatField::SetAcceptable(bool acceptableIn)
{
    numberField->SetAcceptable(acceptableIn);
}

double FloatField::GetValue() const
{
    POMDOG_ASSERT(dataContext != nullptr);
    return dataContext->GetValue();
}

void FloatField::SetValue(double valueIn)
{
    POMDOG_ASSERT(dataContext != nullptr);
    dataContext->SetValue(valueIn);

    POMDOG_ASSERT(numberField != nullptr);
    numberField->SetText(dataContext->ToString());
}

double FloatField::GetMinimum() const noexcept
{
    POMDOG_ASSERT(dataContext != nullptr);
    return dataContext->GetMinimum();
}

void FloatField::SetMinimum(double minimum)
{
    POMDOG_ASSERT(dataContext != nullptr);
    dataContext->SetMinimum(minimum);
}

double FloatField::GetMaximum() const noexcept
{
    POMDOG_ASSERT(dataContext != nullptr);
    return dataContext->GetMaximum();
}

void FloatField::SetMaximum(double maximum)
{
    POMDOG_ASSERT(dataContext != nullptr);
    dataContext->SetMaximum(maximum);
}

int FloatField::GetDecimals() const
{
    POMDOG_ASSERT(dataContext != nullptr);
    return dataContext->GetDecimals();
}

void FloatField::SetDecimals(int precision)
{
    POMDOG_ASSERT(dataContext != nullptr);
    dataContext->SetDecimals(precision);
}

std::string FloatField::GetPlaceholderText() const
{
    return numberField->GetPlaceholderText();
}

void FloatField::SetPlaceholderText(const std::string& textIn)
{
    numberField->SetPlaceholderText(textIn);
}

bool FloatField::HasSelectedText() const
{
    return numberField->HasSelectedText();
}

void FloatField::Deselect()
{
    numberField->Deselect();
}

void FloatField::SetPrependAreaColor(const std::optional<Color>& color)
{
    numberField->SetPrependAreaColor(color);
}

void FloatField::SetPrependAreaString(const std::string& text)
{
    numberField->SetPrependAreaString(text);
}

void FloatField::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    numberField->SetHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment FloatField::GetHorizontalAlignment() const noexcept
{
    return numberField->GetHorizontalAlignment();
}

VerticalAlignment FloatField::GetVerticalAlignment() const noexcept
{
    return numberField->GetVerticalAlignment();
}

void FloatField::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    POMDOG_ASSERT(numberField != nullptr);
    numberField->MarkParentTransformDirty();
}

void FloatField::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();

    POMDOG_ASSERT(numberField != nullptr);
    numberField->MarkParentTransformDirty();
}

void FloatField::OnEnter()
{
    if (numberField != nullptr) {
        numberField->OnEnter();
        numberField->MarkParentTransformDirty();
        numberField->SetParent(shared_from_this());

        valueChangedConn = numberField->ValueChanged.connect([this] {
            this->ValueChanged(dataContext->GetValue());
        });
    }
}

std::shared_ptr<Widget> FloatField::GetChildAt(const Point2D& position)
{
    if (numberField != nullptr) {
        auto bounds = numberField->GetBounds();
        if (bounds.contains(position)) {
            return numberField;
        }
    }
    return nullptr;
}

void FloatField::UpdateAnimation(const Duration& frameDuration)
{
    if (numberField != nullptr) {
        numberField->UpdateAnimation(frameDuration);
    }
}

void FloatField::DoLayout()
{
    if (numberField != nullptr) {
        numberField->SetSize(GetWidth(), GetHeight());
        numberField->DoLayout();
    }
}

void FloatField::Draw(DrawingContext& drawingContext)
{
    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    drawingContext.PushTransform(globalPos);

    if (numberField != nullptr) {
        numberField->Draw(drawingContext);
    }

    drawingContext.PopTransform();
}

} // namespace pomdog::gui
