// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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

} // namespace pomdog
