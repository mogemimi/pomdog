// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SAMPLERSTATEGL4_B8072910_B914_4640_9103_CEA8D030AD4F_HPP
#define POMDOG_SAMPLERSTATEGL4_B8072910_B914_4640_9103_CEA8D030AD4F_HPP

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

using SamplerObjectGL4 = Tagged<GLuint, SamplerState>;

class SamplerStateGL4 final: public NativeSamplerState {
public:
    SamplerStateGL4() = delete;

    explicit SamplerStateGL4(SamplerDescription const& description);

    ~SamplerStateGL4() override;

    ///@copydoc NativeSamplerState
    void Apply(NativeGraphicsContext & graphicsContext, int index) override;

private:
    Optional<SamplerObjectGL4> samplerObject;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SAMPLERSTATEGL4_B8072910_B914_4640_9103_CEA8D030AD4F_HPP)
