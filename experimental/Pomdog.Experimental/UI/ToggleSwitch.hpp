// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "UIElement.hpp"
#include "detail/UIEventConnection.hpp"

namespace Pomdog {
namespace UI {

class ToggleSwitch: public UIElement, public std::enable_shared_from_this<ToggleSwitch> {
public:
    explicit ToggleSwitch(std::shared_ptr<UIEventDispatcher> const& dispatcher);

    bool IsOn() const;
    void IsOn(bool isOn);

    bool IsEnabled() const;
    void IsEnabled(bool isEnabled);

    std::string OnContent() const;
    void OnContent(std::string const& onContent);

    std::string OffContent() const;
    void OffContent(std::string const& offContent);

    UI::HorizontalAlignment HorizontalAlignment() const override { return UI::HorizontalAlignment::Stretch; }
    UI::VerticalAlignment VerticalAlignment() const override { return UI::VerticalAlignment::Top; }

    void OnEnter() override;

    void OnPointerPressed(PointerPoint const& pointerPoint) override;

    void OnPointerReleased(PointerPoint const& pointerPoint) override;

    void OnRenderSizeChanged(int width, int height) override;

    void Draw(DrawingContext & drawingContext) override;

    // Events:
    Signal<void(bool isOn)> Toggled;

private:
    class ToggleSwitchButton {
    public:
        ToggleSwitchButton(std::uint16_t width, std::uint16_t height);

        void Draw(DrawingContext & drawingContext, bool isOn, bool isEnabled);

        std::uint16_t Width;
        std::uint16_t Height;
    };

    ToggleSwitchButton button;
    Detail::UIEventConnection connection;
    std::string onContent;
    std::string offContent;
    bool isOn;
    bool isEnabled;
};

} // namespace UI
} // namespace Pomdog
