// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/experimental/image/image.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

using GIFDuration = std::chrono::duration<long long, std::ratio<1LL, 100LL>>;

class POMDOG_EXPORT GIFImageFrame final {
public:
    std::shared_ptr<pomdog::Image> Image;
    GIFDuration Delay;
};

class POMDOG_EXPORT GIFImage final {
public:
    std::vector<GIFImageFrame> Frames;
    int LoopCount;
};

} // namespace pomdog
