// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/xxhash32.h"
#include "pomdog/utility/xxhash64.h"

namespace pomdog::devgui::detail {

inline constexpr u32 keyNameSeed = 20261013;

} // namespace pomdog::devgui::detail

namespace pomdog::devgui {

/// Identifies a widget among the other widgets of its parent scope.
///
/// A key pairs a name with an index. The name is usually the compile-time hash
/// of a string literal (see `operator""_key`), so building a key reads no string
/// at run time. Code that generates UI from a schema can store enum values in
/// `name` instead. The index tells apart widgets that one call site creates at
/// run time, such as the rows of a list. Use a stable model identifier for the
/// index, such as an entity ID, rather than the row position, so that sorting
/// or filtering the list keeps each row's state.
///
/// Keys are local: the same key under two different scopes names two widgets.
struct Key final {
    u32 name = 0;
    u32 index = 0;

    /// Returns a key with the same name and the given index.
    [[nodiscard]] constexpr Key
    at(u32 indexIn) const noexcept
    {
        return Key{name, indexIn};
    }

    [[nodiscard]] constexpr bool
    operator==(const Key& other) const noexcept = default;
};

/// Identifies a widget across the whole UI.
///
/// A widget ID combines a key with the IDs of its enclosing scopes, so it is
/// unique as long as each key is unique within its scope. Per-widget state such
/// as hover, press, and tree expansion is looked up by this value. Zero means
/// no widget.
///
/// Widget IDs are hashes. They identify widgets within a running program and
/// are not meant to be saved to files.
struct WidgetID final {
    u64 value = 0;

    [[nodiscard]] constexpr bool
    operator==(const WidgetID& other) const noexcept = default;
};

/// Returns the ID of the widget that key names inside the scope parent.
///
/// Combining the same key under different parents gives different IDs, and the
/// whole key, including its index, takes part in the hash. The cost is a few
/// integer operations; no string is read.
[[nodiscard]] constexpr WidgetID
combineID(WidgetID parent, Key key) noexcept
{
    namespace xxh64 = pomdog::hash::detail::xxh64;

    // NOTE: This is the XXH64 finalization for one 8-byte lane, seeded with the
    // parent ID. It mixes every bit of the parent and the key, so `parent ^ key`
    // style collisions between nested scopes cannot happen.
    const u64 lane = (static_cast<u64>(key.name) << 32) | static_cast<u64>(key.index);
    u64 h64 = parent.value + xxh64::prime5 + 8;
    h64 ^= xxh64::round(0, lane);
    h64 = pomdog::hash::detail::rotl64(h64, 27) * xxh64::prime1 + xxh64::prime4;
    return WidgetID{xxh64::avalanche(h64)};
}

} // namespace pomdog::devgui

namespace pomdog::devgui::literals {

POMDOG_CLANG_SUPPRESS_WARNING_PUSH
// FIXME: Suppress the warning about 'consteval' specifier being incompatible with
//        C++ standards before C++20 when compiling with Xcode 16.2.
//        See https://github.com/llvm/llvm-project/issues/138775
POMDOG_CLANG_SUPPRESS_WARNING("-Wc++20-compat")

/// Returns a key whose name is the compile-time xxHash of the literal.
///
/// The operator is consteval, so it cannot hash a string at run time. To tell
/// apart widgets created in a loop, add an index with `Key::at`:
/// ```cpp
/// using namespace pomdog::devgui::literals;
/// devgui::button(ui, "remove"_key.at(entity.uint32()), "Remove");
/// ```
[[nodiscard]] consteval Key
operator""_key(const char* name, size_t length) noexcept
{
    return Key{pomdog::hash::xxh32(name, length, detail::keyNameSeed), 0};
}

POMDOG_CLANG_SUPPRESS_WARNING_POP

} // namespace pomdog::devgui::literals
