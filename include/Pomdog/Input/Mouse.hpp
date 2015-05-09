// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MOUSE_8BFC478A_HPP
#define POMDOG_MOUSE_8BFC478A_HPP

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class MouseState;

class POMDOG_EXPORT Mouse {
public:
    Mouse() = default;
    Mouse(Mouse const&) = delete;
    Mouse & operator=(Mouse const&) = delete;

    virtual ~Mouse() = default;

    virtual MouseState GetState() const = 0;
};

} // namespace Pomdog

#endif // POMDOG_MOUSE_8BFC478A_HPP
