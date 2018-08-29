// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/Image/GifImage.hpp"
#include <optional>
#include <string>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT GifLoader final {
public:
    static std::optional<GifImage> Open(const std::string& filePath);
};

} // namespace Pomdog
