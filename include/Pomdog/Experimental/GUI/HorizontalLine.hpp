// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"

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
