// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstddef>

namespace Pomdog::Detail {

class ShaderBytecode final {
public:
    const void* Code = nullptr;
    std::size_t ByteLength = 0;
};

} // namespace Pomdog::Detail
