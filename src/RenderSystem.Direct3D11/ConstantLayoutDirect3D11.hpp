//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_CONSTANTLAYOUTDIRECT3D11_7DB9A007_8433_4668_8CF5_CA16E4F87F96_HPP
#define POMDOG_CONSTANTLAYOUTDIRECT3D11_7DB9A007_8433_4668_8CF5_CA16E4F87F96_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeConstantLayout.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vector>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

class ConstantBufferDirect3D11;

struct ConstantBufferBindingDirect3D11 {
	std::shared_ptr<ConstantBufferDirect3D11> ConstantBuffer;
	std::string Name;
	std::uint16_t StartSlot;
};

class ConstantLayoutDirect3D11 final: public NativeConstantLayout {
public:
	explicit ConstantLayoutDirect3D11(std::vector<ConstantBufferBindingDirect3D11> && bindings);

	void SetConstantBuffer(std::string const& constantName, std::shared_ptr<NativeConstantBuffer> const& constantBuffer) override;

	void SetConstantBuffer(std::string const& constantName) override;

	void Apply(ID3D11DeviceContext * deviceContext);

private:
	std::vector<ConstantBufferBindingDirect3D11> bindings;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_CONSTANTLAYOUTDIRECT3D11_7DB9A007_8433_4668_8CF5_CA16E4F87F96_HPP)
