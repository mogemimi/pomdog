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
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui::detail {

std::string FloatFieldDataContext::toString() const
{
    switch (precision_) {
    case 0:
        return pomdog::format("{:.1f}", value_);
    case 1:
        return pomdog::format("{:.1f}", value_);
    case 2:
        return pomdog::format("{:.2f}", value_);
    case 3:
        return pomdog::format("{:.3f}", value_);
    case 4:
        return pomdog::format("{:.4f}", value_);
    default:
        break;
    }
    return pomdog::format("{:f}", value_);
}

std::string FloatFieldDataContext::toEditableString(const std::string& text) const
{
    // NOTE: Remove trailing zeros from string.
    auto v = strings::trimRight(text, '0');
    if (strings::hasSuffix(v, ".")) {
        v = std::string_view{v.data(), std::min(v.size() + 1, text.size())};
    }
    return std::string{v};
}

double FloatFieldDataContext::getValue() const noexcept
{
    return value_;
}

void FloatFieldDataContext::setValue(double valueIn)
{
    value_ = valueIn;
}

double FloatFieldDataContext::getMinimum() const noexcept
{
    return minimum_;
}

void FloatFieldDataContext::setMinimum(double minimumIn)
{
    minimum_ = minimumIn;
}

double FloatFieldDataContext::getMaximum() const noexcept
{
    return maximum_;
}

void FloatFieldDataContext::setMaximum(double maximumIn)
{
    maximum_ = maximumIn;
}

int FloatFieldDataContext::getDecimals() const noexcept
{
    return precision_;
}

void FloatFieldDataContext::setDecimals(int precisionIn)
{
    precision_ = precisionIn;
}

double FloatFieldDataContext::getUnit() const noexcept
{
    if ((0 <= precision_) && (precision_ <= 9)) {
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
        return units[precision_];
    }
    return 0.01;
}

void FloatFieldDataContext::incrementValue()
{
    const double unit = getUnit();
    value_ = std::clamp(value_ + unit, minimum_, maximum_);
}

void FloatFieldDataContext::decrementValue()
{
    const double unit = getUnit();
    value_ = std::clamp(value_ - unit, minimum_, maximum_);
}

void FloatFieldDataContext::beginDragging()
{
    startDragValue_ = value_;
}

void FloatFieldDataContext::updateDragging(int amount)
{
    const double unit = getUnit();
    value_ = std::clamp(startDragValue_ + amount * unit, minimum_, maximum_);
}

bool FloatFieldDataContext::textSubmitted(const std::string& text)
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

    value_ = std::clamp(*newValue, minimum_, maximum_);
    return true;
}

} // namespace pomdog::gui::detail

namespace pomdog::gui {

FloatField::FloatField(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
{
    dataContext_ = std::make_shared<detail::FloatFieldDataContext>();
    numberField_ = std::make_shared<NumberField>(dispatcher, dataContext_);
    setSize(numberField_->getWidth(), numberField_->getHeight());
}

void FloatField::setFontWeight(FontWeight fontWeightIn)
{
    numberField_->setFontWeight(fontWeightIn);
}

void FloatField::setFontSize(FontSize fontSizeIn)
{
    numberField_->setFontSize(fontSizeIn);
}

bool FloatField::isEnabled() const
{
    return numberField_->isEnabled();
}

void FloatField::setEnabled(bool enabledIn)
{
    numberField_->setEnabled(enabledIn);
}

bool FloatField::isReadOnly() const
{
    return numberField_->isReadOnly();
}

void FloatField::setReadOnly(bool readOnlyIn)
{
    numberField_->setReadOnly(readOnlyIn);
}

bool FloatField::isAcceptable() const
{
    return numberField_->isAcceptable();
}

void FloatField::setAcceptable(bool acceptableIn)
{
    numberField_->setAcceptable(acceptableIn);
}

double FloatField::getValue() const
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    return dataContext_->getValue();
}

void FloatField::setValue(double valueIn)
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    dataContext_->setValue(valueIn);

    POMDOG_ASSERT(numberField_ != nullptr);
    numberField_->setText(dataContext_->toString());
}

double FloatField::getMinimum() const noexcept
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    return dataContext_->getMinimum();
}

void FloatField::setMinimum(double minimum_)
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    dataContext_->setMinimum(minimum_);
}

double FloatField::getMaximum() const noexcept
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    return dataContext_->getMaximum();
}

void FloatField::setMaximum(double maximum_)
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    dataContext_->setMaximum(maximum_);
}

int FloatField::getDecimals() const
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    return dataContext_->getDecimals();
}

void FloatField::setDecimals(int precision_)
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    dataContext_->setDecimals(precision_);
}

std::string FloatField::getPlaceholderText() const
{
    return numberField_->getPlaceholderText();
}

void FloatField::setPlaceholderText(const std::string& textIn)
{
    numberField_->setPlaceholderText(textIn);
}

bool FloatField::hasSelectedText() const
{
    return numberField_->hasSelectedText();
}

void FloatField::deselect()
{
    numberField_->deselect();
}

void FloatField::setPrependAreaColor(const std::optional<Color>& color)
{
    numberField_->setPrependAreaColor(color);
}

void FloatField::setPrependAreaString(const std::string& text)
{
    numberField_->setPrependAreaString(text);
}

void FloatField::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    numberField_->setHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment FloatField::getHorizontalAlignment() const noexcept
{
    return numberField_->getHorizontalAlignment();
}

VerticalAlignment FloatField::getVerticalAlignment() const noexcept
{
    return numberField_->getVerticalAlignment();
}

void FloatField::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    POMDOG_ASSERT(numberField_ != nullptr);
    numberField_->markParentTransformDirty();
}

void FloatField::markParentTransformDirty()
{
    Widget::markParentTransformDirty();

    POMDOG_ASSERT(numberField_ != nullptr);
    numberField_->markParentTransformDirty();
}

void FloatField::onEnter()
{
    if (numberField_ != nullptr) {
        numberField_->onEnter();
        numberField_->markParentTransformDirty();
        numberField_->setParent(shared_from_this());

        valueChangedConn_ = numberField_->ValueChanged.connect([this] {
            this->ValueChanged(dataContext_->getValue());
        });
    }
}

std::shared_ptr<Widget> FloatField::getChildAt(const Point2D& position)
{
    if (numberField_ != nullptr) {
        auto bounds = numberField_->getBounds();
        if (bounds.contains(position)) {
            return numberField_;
        }
    }
    return nullptr;
}

void FloatField::updateAnimation(const Duration& frameDuration)
{
    if (numberField_ != nullptr) {
        numberField_->updateAnimation(frameDuration);
    }
}

void FloatField::doLayout()
{
    if (numberField_ != nullptr) {
        numberField_->setSize(getWidth(), getHeight());
        numberField_->doLayout();
    }
}

void FloatField::draw(DrawingContext& drawingContext)
{
    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    drawingContext.pushTransform(globalPos);

    if (numberField_ != nullptr) {
        numberField_->draw(drawingContext);
    }

    drawingContext.popTransform();
}

} // namespace pomdog::gui
