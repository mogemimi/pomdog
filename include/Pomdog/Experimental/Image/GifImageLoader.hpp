// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/Image/GifImage.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <vector>
#include <string>

namespace Pomdog {

class POMDOG_EXPORT GifLoader final {
public:
    static Optional<GifImage> Open(const std::string& filePath);
};

} // namespace Pomdog
