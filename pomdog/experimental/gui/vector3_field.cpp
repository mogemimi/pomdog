// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/vector3_field.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/float_field.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
namespace {
constexpr int fieldHeight = 18;
constexpr int fieldPaddingY = 0;
} // namespace

Vector3Field::Vector3Field(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , needToUpdateLayout_(true)
{
    setSize(getWidth(), fieldHeight * 3 + fieldPaddingY * 2);
    setCursor(MouseCursor::ResizeHorizontal);

    xField_ = std::make_shared<FloatField>(dispatcher);
    yField_ = std::make_shared<FloatField>(dispatcher);
    zField_ = std::make_shared<FloatField>(dispatcher);

    xField_->setPrependAreaString("X");
    yField_->setPrependAreaString("Y");
    zField_->setPrependAreaString("Z");

    xField_->setPrependAreaColor(Color{190, 51, 62, 255});
    yField_->setPrependAreaColor(Color{90, 168, 26, 255});
    zField_->setPrependAreaColor(Color{0, 124, 200, 255});
}

bool Vector3Field::isEnabled() const
{
    POMDOG_ASSERT(xField_->isEnabled() == yField_->isEnabled());
    POMDOG_ASSERT(xField_->isEnabled() == zField_->isEnabled());
    return xField_->isEnabled();
}

void Vector3Field::setEnabled(bool enabledIn)
{
    xField_->setEnabled(enabledIn);
    yField_->setEnabled(enabledIn);
    zField_->setEnabled(enabledIn);
}

bool Vector3Field::isReadOnly() const
{
    POMDOG_ASSERT(xField_->isReadOnly() == yField_->isReadOnly());
    POMDOG_ASSERT(xField_->isReadOnly() == zField_->isReadOnly());
    return xField_->isReadOnly();
}

void Vector3Field::setReadOnly(bool readOnlyIn)
{
    xField_->setReadOnly(readOnlyIn);
    yField_->setReadOnly(readOnlyIn);
    zField_->setReadOnly(readOnlyIn);
}

bool Vector3Field::isAcceptable() const
{
    POMDOG_ASSERT(xField_->isAcceptable() == yField_->isAcceptable());
    POMDOG_ASSERT(xField_->isAcceptable() == zField_->isAcceptable());
    return xField_->isAcceptable();
}

void Vector3Field::setAcceptable(bool acceptableIn)
{
    xField_->setAcceptable(acceptableIn);
    yField_->setAcceptable(acceptableIn);
    zField_->setAcceptable(acceptableIn);
}

Vector3 Vector3Field::getValue() const
{
    Vector3 value;
    value.x = static_cast<float>(xField_->getValue());
    value.y = static_cast<float>(yField_->getValue());
    value.z = static_cast<float>(zField_->getValue());
    return value;
}

void Vector3Field::setValue(const Vector3& value)
{
    xField_->setValue(static_cast<double>(value.x));
    yField_->setValue(static_cast<double>(value.y));
    zField_->setValue(static_cast<double>(value.z));
}

Vector3 Vector3Field::getMinimum() const noexcept
{
    Vector3 value;
    value.x = static_cast<float>(xField_->getMinimum());
    value.y = static_cast<float>(yField_->getMinimum());
    value.z = static_cast<float>(zField_->getMinimum());
    return value;
}

void Vector3Field::setMinimum(const Vector3& minimum)
{
    xField_->setMinimum(static_cast<double>(minimum.x));
    yField_->setMinimum(static_cast<double>(minimum.y));
    zField_->setMinimum(static_cast<double>(minimum.z));
}

Vector3 Vector3Field::getMaximum() const noexcept
{
    Vector3 value;
    value.x = static_cast<float>(xField_->getMaximum());
    value.y = static_cast<float>(yField_->getMaximum());
    value.z = static_cast<float>(zField_->getMaximum());
    return value;
}

void Vector3Field::setMaximum(const Vector3& maximum)
{
    xField_->setMaximum(static_cast<double>(maximum.x));
    yField_->setMaximum(static_cast<double>(maximum.y));
    zField_->setMaximum(static_cast<double>(maximum.z));
}

int Vector3Field::getDecimals() const
{
    return xField_->getDecimals();
}

void Vector3Field::setDecimals(int precision)
{
    xField_->setDecimals(precision);
    yField_->setDecimals(precision);
    zField_->setDecimals(precision);
}

void Vector3Field::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    xField_->setHorizontalAlignment(horizontalAlignmentIn);
    yField_->setHorizontalAlignment(horizontalAlignmentIn);
    zField_->setHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment Vector3Field::getHorizontalAlignment() const noexcept
{
    POMDOG_ASSERT(xField_->getHorizontalAlignment() == yField_->getHorizontalAlignment());
    POMDOG_ASSERT(xField_->getHorizontalAlignment() == zField_->getHorizontalAlignment());
    return xField_->getHorizontalAlignment();
}

VerticalAlignment Vector3Field::getVerticalAlignment() const noexcept
{
    POMDOG_ASSERT(xField_->getVerticalAlignment() == yField_->getVerticalAlignment());
    POMDOG_ASSERT(xField_->getVerticalAlignment() == zField_->getVerticalAlignment());
    return xField_->getVerticalAlignment();
}

void Vector3Field::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);
    needToUpdateLayout_ = true;

    POMDOG_ASSERT(xField_ != nullptr);
    POMDOG_ASSERT(yField_ != nullptr);
    POMDOG_ASSERT(zField_ != nullptr);
    xField_->markParentTransformDirty();
    yField_->markParentTransformDirty();
    zField_->markParentTransformDirty();
}

void Vector3Field::markParentTransformDirty()
{
    Widget::markParentTransformDirty();
    needToUpdateLayout_ = true;

    POMDOG_ASSERT(xField_ != nullptr);
    POMDOG_ASSERT(yField_ != nullptr);
    POMDOG_ASSERT(zField_ != nullptr);
    xField_->markParentTransformDirty();
    yField_->markParentTransformDirty();
    zField_->markParentTransformDirty();
}

std::shared_ptr<Widget> Vector3Field::getChildAt(const Point2D& position)
{
    std::array<std::shared_ptr<FloatField>, 3> children = {{xField_, yField_, zField_}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        const auto bounds = child->getBounds();
        if (bounds.contains(position)) {
            return child;
        }
    }
    return nullptr;
}

void Vector3Field::updateAnimation(const Duration& frameDuration)
{
    std::array<std::shared_ptr<FloatField>, 3> children = {{xField_, yField_, zField_}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->updateAnimation(frameDuration);
    }
}

void Vector3Field::onEnter()
{
    std::array<std::shared_ptr<Widget>, 3> children = {{
        xField_,
        yField_,
        zField_,
    }};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->onEnter();
        child->markParentTransformDirty();
        child->setParent(shared_from_this());
    }

    auto onValueChanged = [this]([[maybe_unused]] double) {
        auto value = this->getValue();
        this->ValueChanged(value);
    };
    xValueChangedConn_ = xField_->ValueChanged.connect(onValueChanged);
    yValueChangedConn_ = yField_->ValueChanged.connect(onValueChanged);
    zValueChangedConn_ = zField_->ValueChanged.connect(onValueChanged);
}

void Vector3Field::doLayout()
{
    if (!needToUpdateLayout_) {
        return;
    }
    needToUpdateLayout_ = false;

    setSize(getWidth(), fieldHeight * 3 + fieldPaddingY * 2);

    POMDOG_ASSERT(xField_ != nullptr);
    POMDOG_ASSERT(yField_ != nullptr);
    POMDOG_ASSERT(zField_ != nullptr);

    xField_->setSize(getWidth(), fieldHeight);
    yField_->setSize(getWidth(), fieldHeight);
    zField_->setSize(getWidth(), fieldHeight);

    xField_->setPosition(Point2D{0, (fieldHeight + fieldPaddingY) * 2});
    yField_->setPosition(Point2D{0, fieldHeight + fieldPaddingY});
    zField_->setPosition(Point2D{0, 0});

    xField_->doLayout();
    yField_->doLayout();
    zField_->doLayout();
}

void Vector3Field::draw(DrawingContext& drawingContext)
{
    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    drawingContext.pushTransform(globalPos);

    POMDOG_ASSERT(xField_ != nullptr);
    xField_->draw(drawingContext);

    POMDOG_ASSERT(yField_ != nullptr);
    yField_->draw(drawingContext);

    POMDOG_ASSERT(zField_ != nullptr);
    zField_->draw(drawingContext);

    drawingContext.popTransform();
}

} // namespace pomdog::gui
