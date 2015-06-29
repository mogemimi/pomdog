// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BINARYFILESTREAM_828A8EEF_HPP
#define POMDOG_BINARYFILESTREAM_828A8EEF_HPP

#include <fstream>
#include <cstddef>

namespace Pomdog {
namespace Detail {

struct BinaryFileStream {
    std::ifstream Stream;
    std::size_t SizeInBytes;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_BINARYFILESTREAM_828A8EEF_HPP
