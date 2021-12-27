// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/signals/scoped_connection.hpp"

namespace pomdog::gui {

class HorizontalLine final
    : public Widget
    , public std::enable_shared_from_this<HorizontalLine> {
public:
    explicit HorizontalLine(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void Draw(DrawingContext& drawingContext) override;
};

} // namespace pomdog::gui
