// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstddef>

namespace Pomdog {
namespace Detail {

class ShaderBytecode {
public:
    void const* Code = nullptr;
    std::size_t ByteLength = 0;
};

} // namespace Detail
} // namespace Pomdog
