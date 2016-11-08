// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"

namespace Pomdog {
namespace UI {

class DisclosureTriangleButton final
    : public UIElement
    , public std::enable_shared_from_this<DisclosureTriangleButton> {
public:
    explicit DisclosureTriangleButton(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsOn() const;
    void SetOn(bool isOn);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext & drawingContext) override;

    // Events:
    Signal<void(bool isOn)> Toggled;

private:
    Detail::UIEventConnection connection;
    Rendering::PrimitiveFunctionCommand renderCommand;
    bool isOn;
    bool isEnabled;
};

} // namespace UI
} // namespace Pomdog
