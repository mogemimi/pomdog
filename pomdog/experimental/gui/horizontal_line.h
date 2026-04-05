// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/gui/widget.h"
#include "pomdog/signals/scoped_connection.h"

namespace pomdog::gui {

class HorizontalLine final
    : public Widget
    , public std::enable_shared_from_this<HorizontalLine> {
public:
    explicit HorizontalLine(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void draw(DrawingContext& drawingContext) override;
};

} // namespace pomdog::gui
