// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

enum class TextureAddressMode : u8 {
    Wrap,
    Clamp,
    Mirror,
    Border,
};

} // namespace pomdog::gpu
