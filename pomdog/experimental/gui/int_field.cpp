// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/int_field.h"
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

std::string IntFieldDataContext::toString() const
{
    return std::to_string(value_);
}

std::string IntFieldDataContext::toEditableString(const std::string& text) const
{
    return text;
}

int IntFieldDataContext::getValue() const noexcept
{
    return value_;
}

void IntFieldDataContext::setValue(int valueIn)
{
    value_ = valueIn;
}

void IntFieldDataContext::incrementValue()
{
    constexpr int unit = 1;
    value_ = value_ + unit;
}

void IntFieldDataContext::decrementValue()
{
    constexpr int unit = 1;
    value_ = value_ - unit;
}

void IntFieldDataContext::beginDragging()
{
    startDragValue_ = value_;
}

void IntFieldDataContext::updateDragging(int amount)
{
    constexpr int unit = 1;
    value_ = startDragValue_ + amount * unit;
}

bool IntFieldDataContext::textSubmitted(const std::string& text)
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

    value_ = *newValue;
    return true;
}

} // namespace pomdog::gui::detail

namespace pomdog::gui {

IntField::IntField(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
{
    dataContext_ = std::make_shared<detail::IntFieldDataContext>();
    numberField_ = std::make_shared<NumberField>(dispatcher, dataContext_);
    setSize(numberField_->getWidth(), numberField_->getHeight());
}

void IntField::setFontWeight(FontWeight fontWeightIn)
{
    numberField_->setFontWeight(fontWeightIn);
}

void IntField::setFontSize(FontSize fontSizeIn)
{
    numberField_->setFontSize(fontSizeIn);
}

bool IntField::isEnabled() const
{
    return numberField_->isEnabled();
}

void IntField::setEnabled(bool enabledIn)
{
    numberField_->setEnabled(enabledIn);
}

bool IntField::isReadOnly() const
{
    return numberField_->isReadOnly();
}

void IntField::setReadOnly(bool readOnlyIn)
{
    numberField_->setReadOnly(readOnlyIn);
}

bool IntField::isAcceptable() const
{
    return numberField_->isAcceptable();
}

void IntField::setAcceptable(bool acceptableIn)
{
    numberField_->setAcceptable(acceptableIn);
}

int IntField::getValue() const
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    return dataContext_->getValue();
}

void IntField::setValue(int valueIn)
{
    POMDOG_ASSERT(dataContext_ != nullptr);
    dataContext_->setValue(valueIn);

    POMDOG_ASSERT(numberField_ != nullptr);
    numberField_->setText(dataContext_->toString());
}

std::string IntField::getPlaceholderText() const
{
    return numberField_->getPlaceholderText();
}

void IntField::setPlaceholderText(const std::string& textIn)
{
    numberField_->setPlaceholderText(textIn);
}

bool IntField::hasSelectedText() const
{
    return numberField_->hasSelectedText();
}

void IntField::deselect()
{
    numberField_->deselect();
}

void IntField::setPrependAreaColor(const std::optional<Color>& color)
{
    numberField_->setPrependAreaColor(color);
}

void IntField::setPrependAreaString(const std::string& text)
{
    numberField_->setPrependAreaString(text);
}

void IntField::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    numberField_->setHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment IntField::getHorizontalAlignment() const noexcept
{
    return numberField_->getHorizontalAlignment();
}

VerticalAlignment IntField::getVerticalAlignment() const noexcept
{
    return numberField_->getVerticalAlignment();
}

void IntField::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    POMDOG_ASSERT(numberField_ != nullptr);
    numberField_->markParentTransformDirty();
}

void IntField::markParentTransformDirty()
{
    Widget::markParentTransformDirty();

    POMDOG_ASSERT(numberField_ != nullptr);
    numberField_->markParentTransformDirty();
}

void IntField::onEnter()
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

std::shared_ptr<Widget> IntField::getChildAt(const Point2D& position)
{
    if (numberField_ != nullptr) {
        auto bounds = numberField_->getBounds();
        if (bounds.contains(position)) {
            return numberField_;
        }
    }
    return nullptr;
}

void IntField::updateAnimation(const Duration& frameDuration)
{
    if (numberField_ != nullptr) {
        numberField_->updateAnimation(frameDuration);
    }
}

void IntField::doLayout()
{
    if (numberField_ != nullptr) {
        numberField_->setSize(getWidth(), getHeight());
        numberField_->doLayout();
    }
}

void IntField::draw(DrawingContext& drawingContext)
{
    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    drawingContext.pushTransform(globalPos);

    if (numberField_ != nullptr) {
        numberField_->draw(drawingContext);
    }

    drawingContext.popTransform();
}

} // namespace pomdog::gui
