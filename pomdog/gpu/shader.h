// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"

namespace pomdog::gpu {

/// Shader is a compiled shader program for use in a graphics pipeline.
class POMDOG_EXPORT Shader {
public:
    virtual ~Shader();
};

} // namespace pomdog::gpu
