// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/Image/Image.hpp"
#include <chrono>
#include <memory>
#include <vector>

namespace Pomdog {

using GifDuration = std::chrono::duration<long long, std::ratio<1LL, 100LL>>;

class POMDOG_EXPORT GifImageFrame final {
public:
    std::shared_ptr<Pomdog::Image> Image;
    GifDuration Delay;
};

class POMDOG_EXPORT GifImage final {
public:
    std::vector<GifImageFrame> Frames;
    int LoopCount;
};

} // namespace Pomdog
