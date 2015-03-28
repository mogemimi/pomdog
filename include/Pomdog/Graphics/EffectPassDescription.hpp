// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASSDESCRIPTION_E706BE44_HPP
#define POMDOG_EFFECTPASSDESCRIPTION_E706BE44_HPP

#include "detail/ForwardDeclarations.hpp"
#include "BlendDescription.hpp"
#include "RasterizerDescription.hpp"
#include "VertexBufferBinding.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

class EffectPassDescription final {
public:
    std::vector<VertexBufferBinding> InputElements;
    std::shared_ptr<Shader> VertexShader;
    std::shared_ptr<Shader> PixelShader;
    BlendDescription BlendState;
    RasterizerDescription RasterizerState;
};

}// namespace Pomdog

#endif // POMDOG_EFFECTPASSDESCRIPTION_E706BE44_HPP
