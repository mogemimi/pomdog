// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstddef>

namespace Pomdog {
namespace Detail {

class ShaderBytecode {
public:
    const void* Code = nullptr;
    std::size_t ByteLength = 0;
};

} // namespace Detail
} // namespace Pomdog
