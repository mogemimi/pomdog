// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "DepthStencilStateDirect3D11.hpp"
#include "../RenderSystem.Direct3D11/GraphicsContextDirect3D11.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static D3D11_STENCIL_OP ToStencilOperationDirect3D11(StencilOperation operation)
{
	switch (operation) {
	case StencilOperation::Keep: return D3D11_STENCIL_OP_KEEP;
	case StencilOperation::Zero: return D3D11_STENCIL_OP_ZERO;
	case StencilOperation::Replace: return D3D11_STENCIL_OP_REPLACE;
	case StencilOperation::IncrementSaturation: return D3D11_STENCIL_OP_INCR_SAT;
	case StencilOperation::DecrementSaturation: return D3D11_STENCIL_OP_DECR_SAT;
	case StencilOperation::Invert: return D3D11_STENCIL_OP_INVERT;
	case StencilOperation::Increment: return D3D11_STENCIL_OP_INCR;
	case StencilOperation::Decrement: return D3D11_STENCIL_OP_DECR;
	}
	return D3D11_STENCIL_OP_KEEP;
}
//-----------------------------------------------------------------------
static D3D11_COMPARISON_FUNC ToComparisonFunctionDirect3D11(ComparisonFunction compareFunction)
{
	switch (compareFunction) {
	case ComparisonFunction::Never: return D3D11_COMPARISON_NEVER;
	case ComparisonFunction::Less:  return D3D11_COMPARISON_LESS;
	case ComparisonFunction::Equal: return D3D11_COMPARISON_EQUAL;
	case ComparisonFunction::LessEqual: return D3D11_COMPARISON_LESS_EQUAL;
	case ComparisonFunction::Greater:   return D3D11_COMPARISON_GREATER;
	case ComparisonFunction::NotEqual:  return D3D11_COMPARISON_NOT_EQUAL;
	case ComparisonFunction::GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
	case ComparisonFunction::Always: return D3D11_COMPARISON_ALWAYS;
	}
	return D3D11_COMPARISON_LESS_EQUAL;
}

}// unnamed namespace
//-----------------------------------------------------------------------
DepthStencilStateDirect3D11::DepthStencilStateDirect3D11(ID3D11Device* nativeDevice, DepthStencilDescription const& description)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = (description.DepthBufferEnable ? TRUE : FALSE);
	depthStencilDesc.DepthWriteMask = (description.DepthBufferWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO);
	depthStencilDesc.DepthFunc = ToComparisonFunctionDirect3D11(description.DepthBufferFunction);

	depthStencilDesc.StencilEnable = (description.StencilEnable ? TRUE : FALSE);
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthStencilDesc.BackFace.StencilDepthFailOp = ToStencilOperationDirect3D11(description.CounterClockwiseFace.StencilDepthBufferFail);
	depthStencilDesc.BackFace.StencilFailOp = ToStencilOperationDirect3D11(description.CounterClockwiseFace.StencilFail);
	depthStencilDesc.BackFace.StencilPassOp = ToStencilOperationDirect3D11(description.CounterClockwiseFace.StencilPass);
	depthStencilDesc.BackFace.StencilFunc = ToComparisonFunctionDirect3D11(description.CounterClockwiseFace.StencilFunction);

	depthStencilDesc.FrontFace.StencilDepthFailOp = ToStencilOperationDirect3D11(description.ClockwiseFace.StencilDepthBufferFail);
	depthStencilDesc.FrontFace.StencilFailOp = ToStencilOperationDirect3D11(description.ClockwiseFace.StencilFail);
	depthStencilDesc.FrontFace.StencilPassOp = ToStencilOperationDirect3D11(description.ClockwiseFace.StencilPass);
	depthStencilDesc.FrontFace.StencilFunc = ToComparisonFunctionDirect3D11(description.ClockwiseFace.StencilFunction);

	POMDOG_ASSERT(nativeDevice);
	HRESULT hr = nativeDevice->CreateDepthStencilState(&depthStencilDesc, &nativeDepthStencilState);

	if (FAILED(hr))
	{
		///@todo throw exception
		// error, FUS RO DAH!
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RenderingAPIError,
		//	"failed to create ID3D11DepthStencilState.");
	}
}
//-----------------------------------------------------------------------
void DepthStencilStateDirect3D11::Apply(NativeGraphicsContext & graphicsContext)
{
	POMDOG_ASSERT(static_cast<GraphicsContextDirect3D11*>(&graphicsContext)
		== dynamic_cast<GraphicsContextDirect3D11*>(&graphicsContext));

	auto & nativeGraphicsContext = static_cast<GraphicsContextDirect3D11 &>(graphicsContext);
	auto deviceContext = nativeGraphicsContext.GetDeviceContext();

	POMDOG_ASSERT(deviceContext);
	deviceContext->OMSetDepthStencilState(nativeDepthStencilState.Get(), 0);
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
