// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Keeps a horizontal row open until destroyed. Create one with
/// `devgui::horizontal()`.
class [[nodiscard]] HorizontalScope final {
public:
    explicit HorizontalScope(UI& ui);
    ~HorizontalScope();

    HorizontalScope(const HorizontalScope&) = delete;
    HorizontalScope& operator=(const HorizontalScope&) = delete;
    HorizontalScope(HorizontalScope&&) = delete;
    HorizontalScope& operator=(HorizontalScope&&) = delete;

private:
    UI& ui_;
};

/// Places the widgets created until the returned object goes out of scope side
/// by side in one row.
///
/// A row is a layout scope only. It does not change widget IDs, so moving
/// widgets into or out of a row keeps their state.
[[nodiscard]] HorizontalScope
horizontal(UI& ui);

} // namespace pomdog::devgui
