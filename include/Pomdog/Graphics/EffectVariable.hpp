// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/EffectAnnotation.hpp"
#include <cstdint>
#include <string>

namespace Pomdog {

struct EffectVariable {
    std::string Name;
    EffectAnnotation Annotation;
    std::uint32_t StartOffset;
};

} // namespace Pomdog
