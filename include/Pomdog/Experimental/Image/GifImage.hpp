// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/Image/Image.hpp"
#include <memory>
#include <vector>
#include <chrono>

namespace Pomdog {

using GifDuration = std::chrono::duration<long long, std::ratio<1LL, 100LL>>;

class POMDOG_EXPORT GifImageFrame final {
public:
    std::shared_ptr<Image> Image;
    GifDuration Delay;
};

class POMDOG_EXPORT GifImage final {
public:
    std::vector<GifImageFrame> Frames;
    int LoopCount;
};

} // namespace Pomdog
