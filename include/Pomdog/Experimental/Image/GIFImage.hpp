// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/Image/Image.hpp"
#include <chrono>
#include <memory>
#include <vector>

namespace Pomdog {

using GIFDuration = std::chrono::duration<long long, std::ratio<1LL, 100LL>>;

class POMDOG_EXPORT GIFImageFrame final {
public:
    std::shared_ptr<Pomdog::Image> Image;
    GIFDuration Delay;
};

class POMDOG_EXPORT GIFImage final {
public:
    std::vector<GIFImageFrame> Frames;
    int LoopCount;
};

} // namespace Pomdog
