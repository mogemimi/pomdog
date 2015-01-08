//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "BlendStateDirect3D11.hpp"
#include "../RenderSystem.Direct3D11/GraphicsContextDirect3D11.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static D3D11_BLEND_OP ToBlendFunctionDirect3D11(BlendFunction blendOperation)
{
	switch (blendOperation) {
	case BlendFunction::Add: return D3D11_BLEND_OP_ADD;
	case BlendFunction::Subtract: return D3D11_BLEND_OP_SUBTRACT;
	case BlendFunction::ReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
	case BlendFunction::Min: return D3D11_BLEND_OP_MIN;
	case BlendFunction::Max: return D3D11_BLEND_OP_MAX;
	};
	return D3D11_BLEND_OP_ADD;
}
//-----------------------------------------------------------------------
static D3D11_BLEND ToBlendDirect3D11(Blend blend)
{
	switch (blend) {
	case Blend::Zero: return D3D11_BLEND_ZERO;
	case Blend::One: return D3D11_BLEND_ONE;
	case Blend::SourceColor: return D3D11_BLEND_SRC_COLOR;
	case Blend::InverseSourceColor: return D3D11_BLEND_INV_SRC_COLOR;
	case Blend::SourceAlpha: return D3D11_BLEND_SRC_ALPHA;
	case Blend::InverseSourceAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
	case Blend::DestinationAlpha: return D3D11_BLEND_DEST_ALPHA;
	case Blend::InverseDestinationAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
	case Blend::DestinationColor: return D3D11_BLEND_DEST_COLOR;
	case Blend::InverseDestinationColor: return D3D11_BLEND_INV_DEST_COLOR;
	case Blend::SourceAlphaSaturation: return D3D11_BLEND_SRC_ALPHA_SAT;
	case Blend::BlendFactor: return D3D11_BLEND_BLEND_FACTOR;
	case Blend::InvereseBlendFactor: return D3D11_BLEND_INV_BLEND_FACTOR;
	};
	return D3D11_BLEND_ONE;
}

}// unnamed namespace
//-----------------------------------------------------------------------
BlendStateDirect3D11::BlendStateDirect3D11(ID3D11Device* nativeDevice, BlendDescription const& description)
	: blendFactor(description.BlendFactor.ToVector4())
	, multiSampleMask(static_cast<UINT>(description.MultiSampleMask))
{
	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
	ZeroMemory(&renderTargetBlendDesc, sizeof(renderTargetBlendDesc));

	///@todo badcode
	renderTargetBlendDesc.BlendEnable =
		(Blend::One == description.ColorSourceBlend) &&
		(Blend::One == description.AlphaSourceBlend) &&
		(Blend::Zero == description.ColorDestinationBlend) &&
		(Blend::Zero == description.AlphaDestinationBlend) ? FALSE : TRUE;

	renderTargetBlendDesc.BlendOp = ToBlendFunctionDirect3D11(description.ColorBlendFunction);
	renderTargetBlendDesc.BlendOpAlpha = ToBlendFunctionDirect3D11(description.AlphaBlendFunction);
	renderTargetBlendDesc.DestBlend = ToBlendDirect3D11(description.ColorDestinationBlend);
	renderTargetBlendDesc.DestBlendAlpha = ToBlendDirect3D11(description.AlphaDestinationBlend);
	renderTargetBlendDesc.SrcBlend = ToBlendDirect3D11(description.ColorSourceBlend);
	renderTargetBlendDesc.SrcBlendAlpha = ToBlendDirect3D11(description.AlphaSourceBlend);

	renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	D3D11_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = (description.AlphaToCoverageEnable ? TRUE : FALSE);
	blendDesc.IndependentBlendEnable = FALSE;//TRUE;
	blendDesc.RenderTarget[0] = renderTargetBlendDesc;

	POMDOG_ASSERT(nativeDevice);
	HRESULT hr = nativeDevice->CreateBlendState(&blendDesc, &nativeBlendState);

	if (FAILED(hr))
	{
		///@todo throw exception
		// error, FUS RO DAH!
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RenderingAPIError,
		//	"failed to create ID3D11BlendState.");
	}
}
//-----------------------------------------------------------------------
void BlendStateDirect3D11::Apply(NativeGraphicsContext & graphicsContext)
{
	POMDOG_ASSERT(static_cast<GraphicsContextDirect3D11*>(&graphicsContext)
		== dynamic_cast<GraphicsContextDirect3D11*>(&graphicsContext));

	auto & nativeGraphicsContext = static_cast<GraphicsContextDirect3D11 &>(graphicsContext);
	auto deviceContext = nativeGraphicsContext.GetDeviceContext();

	POMDOG_ASSERT(deviceContext);
	deviceContext->OMSetBlendState(nativeBlendState.Get(), blendFactor.Data(), multiSampleMask);
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
