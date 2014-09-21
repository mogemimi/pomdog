//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkinnedEffect.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"

namespace Pomdog {
namespace {

#include "Shaders/GLSL.Embedded/SkinnedEffect_VS.inc.h"
#include "Shaders/GLSL.Embedded/SkinnedEffect_PS.inc.h"

struct BuiltinEffectSkinningTrait {
	static std::shared_ptr<EffectPass> Create(GraphicsDevice & graphicsDevice)
	{
		using Details::ShaderBytecode;
		ShaderBytecode vertexShaderCode = {Builtin_GLSL_SkinnedEffect_VS, std::strlen(Builtin_GLSL_SkinnedEffect_VS)};
		ShaderBytecode pixelShaderCode = {Builtin_GLSL_SkinnedEffect_PS, std::strlen(Builtin_GLSL_SkinnedEffect_PS)};
		return std::make_shared<EffectPass>(graphicsDevice, vertexShaderCode, pixelShaderCode);
	}
};

}// unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - SkinnedEffect::Impl
#endif
//-----------------------------------------------------------------------
class SkinnedEffect::Impl {
public:
	explicit Impl(GraphicsDevice & graphicsDevice);

	void Apply(GraphicsContext & graphicsContext);

public:
	Matrix4x4 worldViewProjection;
	std::shared_ptr<Texture2D> texture;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<ConstantBufferBinding> constantBuffers;
	std::shared_ptr<InputLayout> inputLayout;
	Color color;
	
	std::array<std::array<Vector4, 2>, SkinnedEffect::MaxBones> bones;
};
//-----------------------------------------------------------------------
SkinnedEffect::Impl::Impl(GraphicsDevice & graphicsDevice)
	: color(Color::White)
{
	effectPass = graphicsDevice.ShaderPool().Create<BuiltinEffectSkinningTrait>(graphicsDevice);
	constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
	inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
}
//-----------------------------------------------------------------------
void SkinnedEffect::Impl::Apply(GraphicsContext & graphicsContext)
{
	struct alignas(16) Constants {
		Matrix4x4 WorldViewProjection;
		Vector4 Color;
	};
	
	Constants constants;
	constants.WorldViewProjection = Matrix4x4::Transpose(worldViewProjection);
	constants.Color = color.ToVector4();

	constantBuffers->Find("Constants")->SetValue(std::move(constants));
	constantBuffers->Find("SkinningConstants")->SetValue(bones);

	graphicsContext.SetTexture(0, texture);
	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetEffectPass(effectPass);
	graphicsContext.SetConstantBuffers(constantBuffers);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - SkinnedEffect
#endif
//-----------------------------------------------------------------------
SkinnedEffect::SkinnedEffect(GraphicsDevice & graphicsDevice)
	: impl(std::make_unique<Impl>(graphicsDevice))
{}
//-----------------------------------------------------------------------
SkinnedEffect::~SkinnedEffect() = default;
//-----------------------------------------------------------------------
void SkinnedEffect::SetWorldViewProjection(Matrix4x4 const& worldViewProjectionIn)
{
	POMDOG_ASSERT(impl);
	impl->worldViewProjection = worldViewProjectionIn;
}
//-----------------------------------------------------------------------
void SkinnedEffect::SetTexture(std::shared_ptr<Texture2D> const& textureIn)
{
	POMDOG_ASSERT(impl);
	impl->texture = textureIn;
}
//-----------------------------------------------------------------------
void SkinnedEffect::SetColor(Color const& colorIn)
{
	POMDOG_ASSERT(impl);
	impl->color = colorIn;
}
//-----------------------------------------------------------------------
void SkinnedEffect::SetBoneTransforms(Matrix3x2 const* boneTransforms, std::size_t count)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(boneTransforms != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(count <= MaxBones);
	
	auto & bones = impl->bones;
	
	for (std::size_t i = 0; i < count; ++i)
	{
		POMDOG_ASSERT(i < bones.size());
		POMDOG_ASSERT(bones[i].size() == 2);
		bones[i][0].X = boneTransforms[i](0, 0);
		bones[i][0].Y = boneTransforms[i](0, 1);
		bones[i][0].Z = boneTransforms[i](1, 0);
		bones[i][0].W = boneTransforms[i](1, 1);
		bones[i][1].X = boneTransforms[i](2, 0);
		bones[i][1].Y = boneTransforms[i](2, 1);
	}
}
//-----------------------------------------------------------------------
void SkinnedEffect::Apply(GraphicsContext & graphicsContext)
{
	POMDOG_ASSERT(impl);
	impl->Apply(graphicsContext);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
