// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CAMERA2D_6E6D82E9_HPP
#define POMDOG_CAMERA2D_6E6D82E9_HPP

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT Camera2D: public Component<Camera2D> {
public:
    // range: [std::numeric_limits<float>::epsilon(), max()] or (0, max()]
    float Zoom = 1;

    // Clipping Planes
    float Near = 0.1f;
    float Far = 1000.0f;

    float NormalizedViewportX = 0.0f;
    float NormalizedViewportY = 0.0f;
    float NormalizedViewportWidth = 1.0f;
    float NormalizedViewportHeight = 1.0f;
};

} // namespace Pomdog

#endif // POMDOG_CAMERA2D_6E6D82E9_HPP
