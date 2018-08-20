// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/EffectAnnotation.hpp"
#include <cstdint>
#include <string>

namespace Pomdog {

struct EffectVariable final {
    std::string Name;
    EffectAnnotation Annotation;
    std::uint32_t StartOffset;
};

} // namespace Pomdog
