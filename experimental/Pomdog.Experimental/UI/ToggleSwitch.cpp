// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/ToggleSwitch.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {
namespace {

const Color toggleOnInnerColor = {162, 122, 16, 255};
const Color toggleOnThumbColor = {252, 252, 250, 255};
const Color toggleOffInnerColor = {160, 160, 160, 255};
const Color toggleOffThumbColor = {81, 81, 81, 255};
const Color disabledInnerColor = {81, 81, 81, 255};
const Color disabledThumbColor = {137, 137, 137, 255};

constexpr float thumbRadius = 7.0f;
constexpr float thumbPadding = 2.0f;
constexpr int segments = 12;

} // unnamed namespace

ToggleSwitch::ToggleSwitch(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
    , isOn(true)
    , isEnabled(true)
{
    SetSize(48, static_cast<int>(thumbRadius) * 2);
    SetCursor(MouseCursor::PointingHand);
}

// MARK: - Properties

bool ToggleSwitch::IsOn() const
{
    return isOn;
}

void ToggleSwitch::SetOn(bool isOnIn)
{
    this->isOn = isOnIn;
}

bool ToggleSwitch::IsEnabled() const
{
    return isEnabled;
}

void ToggleSwitch::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
    if (isEnabled) {
        SetCursor(MouseCursor::PointingHand);
    }
    else {
        ResetCursor();
    }
}

HorizontalAlignment ToggleSwitch::GetHorizontalAlignment() const noexcept
{
    return UI::HorizontalAlignment::Right;
}

VerticalAlignment ToggleSwitch::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Top;
}

// MARK: - Events

void ToggleSwitch::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());
}

void ToggleSwitch::OnPointerPressed(const PointerPoint& pointerPoint)
{
}

void ToggleSwitch::OnPointerReleased(const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isOn = !isOn;
    Toggled(isOn);
}

void ToggleSwitch::Draw(DrawingContext & drawingContext)
{
    auto transform = GetTransform() * drawingContext.Top();

    renderCommand.SetInvoker([this, transform](PolygonBatch & polygonBatch) {
        Color innerColor = {109, 109, 109, 255};
        Color thumbColor = Color::White;

        if (!isEnabled) {
            innerColor = disabledInnerColor;
            thumbColor = disabledThumbColor;
        }

        if (isEnabled) {
            if (isOn) {
                innerColor = toggleOnInnerColor;
                thumbColor = toggleOnThumbColor;
            }
            else {
                innerColor = toggleOffInnerColor;
                thumbColor = toggleOffThumbColor;
            }
        }

        const auto transformOffset = Vector2{transform(2, 0), transform(2, 1)};

        polygonBatch.DrawArc(
            transformOffset + Vector2{
                static_cast<float>(GetWidth()) - (thumbRadius + thumbPadding),
                (thumbRadius + thumbPadding)
            },
            thumbRadius + thumbPadding,
            MathHelper::ToRadians(270.0f),
            MathHelper::ToRadians(180.0f),
            segments,
            innerColor);

        polygonBatch.DrawArc(
            transformOffset + Vector2{
                (thumbRadius + thumbPadding),
                (thumbRadius + thumbPadding)
            },
            thumbRadius + thumbPadding,
            MathHelper::ToRadians(90.0f),
            MathHelper::ToRadians(180.0f),
            segments,
            innerColor);

        polygonBatch.DrawRectangle(
            transform,
            Vector2{
                (thumbRadius + thumbPadding),
                0.0f
            },
            GetWidth() - (thumbRadius + thumbPadding) * 2.0f,
            (thumbRadius + thumbPadding) * 2.0f,
            innerColor);

        if (isOn) {
            polygonBatch.DrawCircle(
                transformOffset + Vector2{
                    static_cast<float>(GetWidth()) - (thumbRadius + thumbPadding),
                    (thumbRadius + thumbPadding)
                },
                thumbRadius,
                segments,
                thumbColor);
        }
        else {
            polygonBatch.DrawCircle(
                transformOffset + Vector2{
                    (thumbRadius + thumbPadding),
                    (thumbRadius + thumbPadding)
                },
                thumbRadius,
                segments,
                thumbColor);
        }
    });
    drawingContext.PushCommand(renderCommand);

    spriteCommand.SetDrawOrder(5.0f);
    spriteCommand.SetFont(drawingContext.GetFont(FontWeight::Bold, FontSize::Small));

    if (isOn) {
        auto textPosition = Vector2{transform(2, 0), transform(2, 1)}
            + Vector2{thumbPadding, thumbPadding}
            + Vector2{9.0f, 4.0f};
        spriteCommand.SetPosition(textPosition);
        spriteCommand.SetColor(toggleOnThumbColor);
        spriteCommand.SetText("ON");
    }
    else {
        auto textPosition = Vector2{transform(2, 0), transform(2, 1)}
            + Vector2{static_cast<float>(GetWidth()) - (thumbRadius + thumbPadding), thumbPadding}
            + Vector2{-19.0f, 4.0f};
        spriteCommand.SetPosition(textPosition);
        spriteCommand.SetColor(Color{250, 250, 250, 255});
        spriteCommand.SetText("OFF");
    }
    drawingContext.PushCommand(spriteCommand);
}

} // namespace UI
} // namespace Pomdog
