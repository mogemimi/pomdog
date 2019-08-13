// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstddef>
#include <fstream>

namespace Pomdog::Detail {

struct BinaryFileStream {
    std::ifstream Stream;
    std::size_t SizeInBytes;
};

} // namespace Pomdog::Detail
