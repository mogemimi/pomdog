//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GradientPlane.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"
#include "Pomdog/Graphics/VertexDeclaration.hpp"

namespace Pomdog {
namespace SceneEditor {
namespace {

// Built-in shaders
#include "Pomdog.Experimental/Graphics/Shaders/GLSL.Embedded/LineBatch_VS.inc.h"
#include "Pomdog.Experimental/Graphics/Shaders/GLSL.Embedded/LineBatch_PS.inc.h"

}// unnamed namespace
//-----------------------------------------------------------------------
GradientPlane::GradientPlane(std::shared_ptr<GameHost> const& gameHost)
	: graphicsContext(gameHost->GraphicsContext())
{
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		using PositionColor = CustomVertex<Vector3, Vector4>;
		
		Color colorUpper {90, 86, 80, 255};
		Color colorLower {57, 67, 86, 255};
		
		std::array<PositionColor, 6> const verticesCombo = {
			Vector3(-1.0f, -1.0f, 0.0f), colorLower.ToVector4(),// [0]
			Vector3(-1.0f,  1.0f, 0.0f), colorUpper.ToVector4(),// [1]
			Vector3( 1.0f,  1.0f, 0.0f), colorUpper.ToVector4(),// [2]
			Vector3( 1.0f,  1.0f, 0.0f), colorUpper.ToVector4(),// [2]
			Vector3( 1.0f, -1.0f, 0.0f), colorLower.ToVector4(),// [3]
			Vector3(-1.0f, -1.0f, 0.0f), colorLower.ToVector4(),// [0]
		};
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			verticesCombo.data(), verticesCombo.size(),
			PositionColor::Declaration().StrideBytes(), BufferUsage::Immutable);
	}
	{
		using Details::ShaderBytecode;
		ShaderBytecode vertexShader;
		vertexShader.Code = Builtin_GLSL_LineBatch_VS;
		vertexShader.ByteLength = std::strlen(Builtin_GLSL_LineBatch_VS);

		ShaderBytecode pixelShader;
		pixelShader.Code = Builtin_GLSL_LineBatch_PS;
		pixelShader.ByteLength = std::strlen(Builtin_GLSL_LineBatch_PS);

		effectPass = std::make_shared<EffectPass>(graphicsDevice, vertexShader, pixelShader);
		constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass,
			VertexDeclaration{{VertexElementFormat::Float3, VertexElementFormat::Float4}});
	}
}
//-----------------------------------------------------------------------
void GradientPlane::Draw()
{
	auto parameter = constantBuffers->Find("TransformMatrix");
	parameter->SetValue(Matrix4x4::Identity);

	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	graphicsContext->SetEffectPass(effectPass);
	graphicsContext->SetConstantBuffers(constantBuffers);
	graphicsContext->Draw(PrimitiveTopology::TriangleList, vertexBuffer->VertexCount());
}
//-----------------------------------------------------------------------
}// namespace SceneEditor
}// namespace Pomdog
