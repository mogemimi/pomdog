// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Detail {
namespace GL4 {

using SamplerObjectGL4 = Tagged<GLuint, SamplerState>;

class SamplerStateGL4 final : public NativeSamplerState {
public:
    explicit SamplerStateGL4(const SamplerDescription& description);

    ~SamplerStateGL4() override;

    void Apply(int index);

private:
    Optional<SamplerObjectGL4> samplerObject;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
