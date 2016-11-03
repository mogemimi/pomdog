// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"

namespace Pomdog {
namespace UI {

class ToggleSwitch final
    : public UIElement
    , public std::enable_shared_from_this<ToggleSwitch> {
public:
    explicit ToggleSwitch(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsOn() const;
    void SetOn(bool isOn);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    std::string GetOnContent() const;
    void SetOnContent(const std::string& onContent);

    std::string GetOffContent() const;
    void SetOffContent(const std::string& offContent);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void OnRenderSizeChanged(int width, int height) override;

    void Draw(DrawingContext & drawingContext) override;

    // Events:
    Signal<void(bool isOn)> Toggled;

private:
    class ToggleSwitchButton {
    public:
        ToggleSwitchButton(int width, int height);

        void Draw(DrawingContext & drawingContext, bool isOn, bool isEnabled);

        int Width;
        int Height;
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
