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
    , needToUpdateLayout(true)
{
    SetSize(GetWidth(), fieldHeight * 3 + fieldPaddingY * 2);
    SetCursor(MouseCursor::ResizeHorizontal);

    xField = std::make_shared<FloatField>(dispatcher);
    yField = std::make_shared<FloatField>(dispatcher);
    zField = std::make_shared<FloatField>(dispatcher);

    xField->SetPrependAreaString("X");
    yField->SetPrependAreaString("Y");
    zField->SetPrependAreaString("Z");

    xField->SetPrependAreaColor(Color{190, 51, 62, 255});
    yField->SetPrependAreaColor(Color{90, 168, 26, 255});
    zField->SetPrependAreaColor(Color{0, 124, 200, 255});
}

bool Vector3Field::IsEnabled() const
{
    POMDOG_ASSERT(xField->IsEnabled() == yField->IsEnabled());
    POMDOG_ASSERT(xField->IsEnabled() == zField->IsEnabled());
    return xField->IsEnabled();
}

void Vector3Field::SetEnabled(bool enabledIn)
{
    xField->SetEnabled(enabledIn);
    yField->SetEnabled(enabledIn);
    zField->SetEnabled(enabledIn);
}

bool Vector3Field::IsReadOnly() const
{
    POMDOG_ASSERT(xField->IsReadOnly() == yField->IsReadOnly());
    POMDOG_ASSERT(xField->IsReadOnly() == zField->IsReadOnly());
    return xField->IsReadOnly();
}

void Vector3Field::SetReadOnly(bool readOnlyIn)
{
    xField->SetReadOnly(readOnlyIn);
    yField->SetReadOnly(readOnlyIn);
    zField->SetReadOnly(readOnlyIn);
}

bool Vector3Field::IsAcceptable() const
{
    POMDOG_ASSERT(xField->IsAcceptable() == yField->IsAcceptable());
    POMDOG_ASSERT(xField->IsAcceptable() == zField->IsAcceptable());
    return xField->IsAcceptable();
}

void Vector3Field::SetAcceptable(bool acceptableIn)
{
    xField->SetAcceptable(acceptableIn);
    yField->SetAcceptable(acceptableIn);
    zField->SetAcceptable(acceptableIn);
}

Vector3 Vector3Field::GetValue() const
{
    Vector3 value;
    value.x = static_cast<float>(xField->GetValue());
    value.y = static_cast<float>(yField->GetValue());
    value.z = static_cast<float>(zField->GetValue());
    return value;
}

void Vector3Field::SetValue(const Vector3& value)
{
    xField->SetValue(static_cast<double>(value.x));
    yField->SetValue(static_cast<double>(value.y));
    zField->SetValue(static_cast<double>(value.z));
}

Vector3 Vector3Field::GetMinimum() const noexcept
{
    Vector3 value;
    value.x = static_cast<float>(xField->GetMinimum());
    value.y = static_cast<float>(yField->GetMinimum());
    value.z = static_cast<float>(zField->GetMinimum());
    return value;
}

void Vector3Field::SetMinimum(const Vector3& minimum)
{
    xField->SetMinimum(static_cast<double>(minimum.x));
    yField->SetMinimum(static_cast<double>(minimum.y));
    zField->SetMinimum(static_cast<double>(minimum.z));
}

Vector3 Vector3Field::GetMaximum() const noexcept
{
    Vector3 value;
    value.x = static_cast<float>(xField->GetMaximum());
    value.y = static_cast<float>(yField->GetMaximum());
    value.z = static_cast<float>(zField->GetMaximum());
    return value;
}

void Vector3Field::SetMaximum(const Vector3& maximum)
{
    xField->SetMaximum(static_cast<double>(maximum.x));
    yField->SetMaximum(static_cast<double>(maximum.y));
    zField->SetMaximum(static_cast<double>(maximum.z));
}

int Vector3Field::GetDecimals() const
{
    return xField->GetDecimals();
}

void Vector3Field::SetDecimals(int precision)
{
    xField->SetDecimals(precision);
    yField->SetDecimals(precision);
    zField->SetDecimals(precision);
}

void Vector3Field::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    xField->SetHorizontalAlignment(horizontalAlignmentIn);
    yField->SetHorizontalAlignment(horizontalAlignmentIn);
    zField->SetHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment Vector3Field::GetHorizontalAlignment() const noexcept
{
    POMDOG_ASSERT(xField->GetHorizontalAlignment() == yField->GetHorizontalAlignment());
    POMDOG_ASSERT(xField->GetHorizontalAlignment() == zField->GetHorizontalAlignment());
    return xField->GetHorizontalAlignment();
}

VerticalAlignment Vector3Field::GetVerticalAlignment() const noexcept
{
    POMDOG_ASSERT(xField->GetVerticalAlignment() == yField->GetVerticalAlignment());
    POMDOG_ASSERT(xField->GetVerticalAlignment() == zField->GetVerticalAlignment());
    return xField->GetVerticalAlignment();
}

void Vector3Field::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);
    needToUpdateLayout = true;

    POMDOG_ASSERT(xField != nullptr);
    POMDOG_ASSERT(yField != nullptr);
    POMDOG_ASSERT(zField != nullptr);
    xField->MarkParentTransformDirty();
    yField->MarkParentTransformDirty();
    zField->MarkParentTransformDirty();
}

void Vector3Field::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();
    needToUpdateLayout = true;

    POMDOG_ASSERT(xField != nullptr);
    POMDOG_ASSERT(yField != nullptr);
    POMDOG_ASSERT(zField != nullptr);
    xField->MarkParentTransformDirty();
    yField->MarkParentTransformDirty();
    zField->MarkParentTransformDirty();
}

std::shared_ptr<Widget> Vector3Field::GetChildAt(const Point2D& position)
{
    std::array<std::shared_ptr<FloatField>, 3> children = {{xField, yField, zField}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        const auto bounds = child->GetBounds();
        if (bounds.contains(position)) {
            return child;
        }
    }
    return nullptr;
}

void Vector3Field::UpdateAnimation(const Duration& frameDuration)
{
    std::array<std::shared_ptr<FloatField>, 3> children = {{xField, yField, zField}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->UpdateAnimation(frameDuration);
    }
}

void Vector3Field::OnEnter()
{
    std::array<std::shared_ptr<Widget>, 3> children = {{
        xField,
        yField,
        zField,
    }};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->OnEnter();
        child->MarkParentTransformDirty();
        child->SetParent(shared_from_this());
    }

    auto onValueChanged = [this]([[maybe_unused]] double) {
        auto value = this->GetValue();
        this->ValueChanged(value);
    };
    xValueChangedConn = xField->ValueChanged.connect(onValueChanged);
    yValueChangedConn = yField->ValueChanged.connect(onValueChanged);
    zValueChangedConn = zField->ValueChanged.connect(onValueChanged);
}

void Vector3Field::DoLayout()
{
    if (!needToUpdateLayout) {
        return;
    }
    needToUpdateLayout = false;

    SetSize(GetWidth(), fieldHeight * 3 + fieldPaddingY * 2);

    POMDOG_ASSERT(xField != nullptr);
    POMDOG_ASSERT(yField != nullptr);
    POMDOG_ASSERT(zField != nullptr);

    xField->SetSize(GetWidth(), fieldHeight);
    yField->SetSize(GetWidth(), fieldHeight);
    zField->SetSize(GetWidth(), fieldHeight);

    xField->SetPosition(Point2D{0, (fieldHeight + fieldPaddingY) * 2});
    yField->SetPosition(Point2D{0, fieldHeight + fieldPaddingY});
    zField->SetPosition(Point2D{0, 0});

    xField->DoLayout();
    yField->DoLayout();
    zField->DoLayout();
}

void Vector3Field::Draw(DrawingContext& drawingContext)
{
    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    drawingContext.PushTransform(globalPos);

    POMDOG_ASSERT(xField != nullptr);
    xField->Draw(drawingContext);

    POMDOG_ASSERT(yField != nullptr);
    yField->Draw(drawingContext);

    POMDOG_ASSERT(zField != nullptr);
    zField->Draw(drawingContext);

    drawingContext.PopTransform();
}

} // namespace pomdog::gui
