// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstddef>

namespace Pomdog {
namespace Detail {

class ShaderBytecode {
public:
    void const* Code;
    std::size_t ByteLength;
};

} // namespace Detail
} // namespace Pomdog
