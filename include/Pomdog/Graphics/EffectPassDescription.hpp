// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASSDESCRIPTION_E706BE44_0599_4E5B_B49F_BC11B1C2E177_HPP
#define POMDOG_EFFECTPASSDESCRIPTION_E706BE44_0599_4E5B_B49F_BC11B1C2E177_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "VertexBufferBinding.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

class EffectPassDescription final {
public:
	std::vector<VertexBufferBinding> InputElements;
	std::shared_ptr<Shader> VertexShader;
	std::shared_ptr<Shader> PixelShader;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASSDESCRIPTION_E706BE44_0599_4E5B_B49F_BC11B1C2E177_HPP)
