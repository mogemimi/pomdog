// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INPUTLAYOUTDIRECT3D11_93CA68F0_7EE1_4BC3_9B42_D90E5CB7E839_HPP
#define POMDOG_INPUTLAYOUTDIRECT3D11_93CA68F0_7EE1_4BC3_9B42_D90E5CB7E839_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vector>
#include <string>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class ShaderBytecode;

namespace Direct3D11 {

struct InputLayoutHelper final {
	static Microsoft::WRL::ComPtr<ID3D11InputLayout>
	CreateInputLayout(ID3D11Device* device,
		ShaderBytecode const& vertexShaderBytecode,
		std::vector<VertexBufferBinding> const& vertexBufferBindings);
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_INPUTLAYOUTDIRECT3D11_93CA68F0_7EE1_4BC3_9B42_D90E5CB7E839_HPP)
