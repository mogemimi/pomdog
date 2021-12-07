// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/signals/scoped_connection.hpp"

namespace Pomdog::GUI {

class HorizontalLine final
    : public Widget
    , public std::enable_shared_from_this<HorizontalLine> {
public:
    explicit HorizontalLine(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void Draw(DrawingContext& drawingContext) override;
};

} // namespace Pomdog::GUI
