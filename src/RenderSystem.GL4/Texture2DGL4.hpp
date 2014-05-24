//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_TEXTURE2DGL4_ACA56E6B_8FDD_4D8A_AA7E_4AE636B271ED_HPP
#define POMDOG_SRC_GL4_TEXTURE2DGL4_ACA56E6B_8FDD_4D8A_AA7E_4AE636B271ED_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <Pomdog/Utility/detail/Tagged.hpp>
#include "../RenderSystem/NativeTexture2D.hpp"
#include "../Utility/Optional.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using Texture2DObjectGL4 = Tagged<GLuint, Texture2D>;

class Texture2DGL4 final: public NativeTexture2D {
public:
	Texture2DGL4(std::uint32_t pixelWidth, std::uint32_t pixelHeight,
		std::uint32_t levelCount, SurfaceFormat format);
	
	~Texture2DGL4() override;
	
	void SetData(std::uint32_t pixelWidth, std::uint32_t pixelHeight,
		std::uint32_t levelCount, SurfaceFormat format, void const* pixelData) override;
	
	///@copydoc NativeTexture2D
	void Apply(std::uint32_t index) override;
	
	void GenerateMipmap();
	
	Texture2DObjectGL4 const& NativeHandle() const;
	
private:
	Optional<Texture2DObjectGL4> textureObject;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_TEXTURE2DGL4_ACA56E6B_8FDD_4D8A_AA7E_4AE636B271ED_HPP)
