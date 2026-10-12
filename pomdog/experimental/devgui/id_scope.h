// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/devgui/key.h"

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Keeps an ID scope open until destroyed. Create one with `devgui::pushID()`.
class [[nodiscard]] IDScope final {
public:
    IDScope(UI& ui, Key key);
    ~IDScope();

    IDScope(const IDScope&) = delete;
    IDScope& operator=(const IDScope&) = delete;
    IDScope(IDScope&&) = delete;
    IDScope& operator=(IDScope&&) = delete;

private:
    UI& ui_;
};

/// Opens an ID scope named key that lasts until the returned object goes out
/// of scope.
///
/// Widgets inside the scope get IDs relative to it, so the same keys can be
/// reused for each item of a list:
/// ```cpp
/// for (const auto& emitter : emitters) {
///     auto id = devgui::pushID(ui, "emitter"_key.at(emitter.id));
///     devgui::dragFloat(ui, "rate"_key, "Rate", emitter.rate, 0.1f);
/// }
/// ```
[[nodiscard]] IDScope
pushID(UI& ui, Key key);

} // namespace pomdog::devgui
