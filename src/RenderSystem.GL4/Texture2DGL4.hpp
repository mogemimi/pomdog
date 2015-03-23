// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTURE2DGL4_ACA56E6B_8FDD_4D8A_AA7E_4AE636B271ED_HPP
#define POMDOG_TEXTURE2DGL4_ACA56E6B_8FDD_4D8A_AA7E_4AE636B271ED_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

using Texture2DObjectGL4 = Tagged<GLuint, Texture2D>;

class Texture2DGL4 final: public NativeTexture2D {
public:
	Texture2DGL4(std::int32_t pixelWidth, std::int32_t pixelHeight,
		std::int32_t levelCount, SurfaceFormat format);

	~Texture2DGL4() override;

	void SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
		std::int32_t levelCount, SurfaceFormat format, void const* pixelData) override;

	///@copydoc NativeTexture2D
	void Apply(std::uint32_t index) override;

	void GenerateMipmap();

	Texture2DObjectGL4 const& NativeHandle() const;

private:
	Optional<Texture2DObjectGL4> textureObject;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTURE2DGL4_ACA56E6B_8FDD_4D8A_AA7E_4AE636B271ED_HPP)
