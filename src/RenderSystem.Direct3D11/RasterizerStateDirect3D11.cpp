//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "RasterizerStateDirect3D11.hpp"
#include "../RenderSystem.Direct3D11/GraphicsContextDirect3D11.hpp"
#include "Pomdog/Graphics/RasterizerDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static D3D11_CULL_MODE ToCullModeDirect3D11(CullMode cullMode)
{
	switch (cullMode) {
	case CullMode::ClockwiseFace: return D3D11_CULL_FRONT;
	case CullMode::CounterClockwiseFace: return D3D11_CULL_BACK;
	case CullMode::None: return D3D11_CULL_NONE;
	};
	return D3D11_CULL_BACK;
}
//-----------------------------------------------------------------------
static D3D11_FILL_MODE ToFillModeDirect3D11(FillMode fillMode)
{
	switch (fillMode) {
	case FillMode::WireFrame: return D3D11_FILL_WIREFRAME;
	case FillMode::Solid: return D3D11_FILL_SOLID;
	};
	return D3D11_FILL_SOLID;
}

}// unnamed namespace
//-----------------------------------------------------------------------
RasterizerStateDirect3D11::RasterizerStateDirect3D11(ID3D11Device* nativeDevice, RasterizerDescription const& description)
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	rasterizerDesc.CullMode = ToCullModeDirect3D11(description.CullMode);
	rasterizerDesc.FillMode = ToFillModeDirect3D11(description.FillMode);
	rasterizerDesc.FrontCounterClockwise = FALSE;

	rasterizerDesc.DepthBias = static_cast<INT>(description.DepthBias);
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = static_cast<FLOAT>(description.SlopeScaledDepthBias);
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.MultisampleEnable = (description.MultisampleEnable ? TRUE : FALSE);
	rasterizerDesc.ScissorEnable = (description.ScissorTestEnable ? TRUE : FALSE);

	POMDOG_ASSERT(!rasterizerDesc.AntialiasedLineEnable
		|| (rasterizerDesc.AntialiasedLineEnable && !rasterizerDesc.MultisampleEnable));

	POMDOG_ASSERT(nativeDevice);
	HRESULT hr = nativeDevice->CreateRasterizerState(&rasterizerDesc, &nativeRasterizerState);

	if (FAILED(hr))
	{
		///@todo throw exception
		// error, FUS RO DAH!
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RenderingAPIError,
		//	"failed to create ID3D11RasterizerState.");
	}
}
//-----------------------------------------------------------------------
void RasterizerStateDirect3D11::Apply(NativeGraphicsContext & graphicsContext)
{
	POMDOG_ASSERT(static_cast<GraphicsContextDirect3D11*>(&graphicsContext)
		== dynamic_cast<GraphicsContextDirect3D11*>(&graphicsContext));
	auto & nativeGraphicsContext = static_cast<GraphicsContextDirect3D11 &>(graphicsContext);
	auto deviceContext = nativeGraphicsContext.GetDeviceContext();

	POMDOG_ASSERT(deviceContext);
	deviceContext->RSSetState(nativeRasterizerState.Get());
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
