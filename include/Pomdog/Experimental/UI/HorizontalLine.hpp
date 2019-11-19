// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/UI/UIElement.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"

namespace Pomdog::UI {

class HorizontalLine final
    : public UIElement
    , public std::enable_shared_from_this<HorizontalLine> {
public:
    explicit HorizontalLine(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void Draw(DrawingContext& drawingContext) override;
};

} // namespace Pomdog::UI
