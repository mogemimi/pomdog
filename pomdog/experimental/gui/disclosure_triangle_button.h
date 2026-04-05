// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/gui/widget.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

namespace pomdog::gui {

class DisclosureTriangleButton final
    : public Widget
    , public std::enable_shared_from_this<DisclosureTriangleButton> {
public:
    explicit DisclosureTriangleButton(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    [[nodiscard]] bool
    isOn() const;

    void
    setOn(bool isOn);

    [[nodiscard]] bool
    isEnabled() const;

    void
    setEnabled(bool isEnabled);

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    onEnter() override;

    void
    onPointerPressed(const PointerPoint& pointerPoint) override;

    void
    onPointerReleased(const PointerPoint& pointerPoint) override;

    void
    draw(DrawingContext& drawingContext) override;

    Signal<void(bool isOn)> Toggled;

private:
    ScopedConnection connection_;
    bool isOn_;
    bool isEnabled_;
};

} // namespace pomdog::gui
