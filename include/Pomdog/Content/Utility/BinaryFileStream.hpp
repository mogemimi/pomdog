// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstddef>
#include <fstream>

namespace Pomdog {
namespace Detail {

struct BinaryFileStream {
    std::ifstream Stream;
    std::size_t SizeInBytes;
};

} // namespace Detail
} // namespace Pomdog
