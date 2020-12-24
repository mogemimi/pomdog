// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/FloatField.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/PushButton.hpp"
#include "Pomdog/Experimental/GUI/TextBlock.hpp"
#include "Pomdog/Experimental/GUI/TextEdit.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <array>

namespace Pomdog::GUI::Detail {

std::string FloatFieldDataContext::ToString() const
{
    switch (precision) {
    case 0:
        return StringHelper::Format("%.1lf", value);
    case 1:
        return StringHelper::Format("%.1lf", value);
    case 2:
        return StringHelper::Format("%.2lf", value);
    case 3:
        return StringHelper::Format("%.3lf", value);
    case 4:
        return StringHelper::Format("%.4lf", value);
    default:
        break;
    }
    return StringHelper::Format("%lf", value);
}

std::string FloatFieldDataContext::ToEditableString(const std::string& text) const
{
    // NOTE: Remove trailing zeros from string.
    auto v = StringHelper::TrimRight(text, '0');
    if (StringHelper::HasSuffix(v, ".")) {
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

} // namespace Pomdog::GUI::Detail

namespace Pomdog::GUI {

FloatField::FloatField(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
{
    dataContext = std::make_shared<Detail::FloatFieldDataContext>();
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

        valueChangedConn = numberField->ValueChanged.Connect([this] {
            this->ValueChanged(dataContext->GetValue());
        });
    }
}

std::shared_ptr<Widget> FloatField::GetChildAt(const Point2D& position)
{
    if (numberField != nullptr) {
        auto bounds = numberField->GetBounds();
        if (bounds.Contains(position)) {
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

} // namespace Pomdog::GUI
