// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

enum class Keys : std::uint8_t;
class KeyboardState;

class POMDOG_EXPORT Keyboard {
public:
    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;

    virtual ~Keyboard();

    virtual KeyboardState GetState() const = 0;

    Signal<void(Keys key)> KeyDown;

    Signal<void(Keys key)> KeyUp;

    Signal<void(const std::string& text)> TextInput;
};

} // namespace Pomdog
