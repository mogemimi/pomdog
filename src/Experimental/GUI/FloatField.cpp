// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/FloatField.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/NumberField.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/PushButton.hpp"
#include "Pomdog/Experimental/GUI/TextBlock.hpp"
#include "Pomdog/Experimental/GUI/TextEdit.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <array>

namespace Pomdog::GUI {
namespace Detail {

class FloatFieldDataContext final : public NumberFieldDataContext {
public:
    std::string ToString() const override
    {
        return StringHelper::Format("%.3lf", value);
    }

    std::string ToEditableString(const std::string& text) const override
    {
        // NOTE: Remove trailing zeros from string.
        auto v = StringHelper::TrimRight(text, '0');
        if (StringHelper::HasSuffix(v, ".")) {
            v = std::string_view{v.data(), std::min(v.size() + 1, text.size())};
        }
        return std::string{v};
    }

    double GetValue() const noexcept
    {
        return value;
    }

    void SetValue(double valueIn)
    {
        value = valueIn;
    }

    void IncrementValue() override
    {
        constexpr double unit = 0.01;
        value = value + unit;
    }

    void DecrementValue() override
    {
        constexpr double unit = 0.01;
        value = value - unit;
    }

    void BeginDragging() override
    {
        startDragValue = value;
    }

    void UpdateDragging(int amount) override
    {
        constexpr double unit = 0.01;
        value = startDragValue + amount * unit;
    }

    bool TextSubmitted(const std::string& text) override
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

        value = *newValue;
        return true;
    }

private:
    double value = 0.0;
    double startDragValue = 0.0;
};

} // namespace Detail

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
