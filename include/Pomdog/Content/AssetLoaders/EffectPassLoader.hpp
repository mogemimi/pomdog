//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EFFECTPASSLOADER_C9744EEA_2F5B_46D6_8880_B11D2DF37382_HPP
#define POMDOG_EFFECTPASSLOADER_C9744EEA_2F5B_46D6_8880_B11D2DF37382_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Pomdog {

class EffectPass;
class GraphicsDevice;
class VertexBufferBinding;

namespace Details {
class AssetLoaderContext;
}// namespace Details

namespace AssetLoaders {

class POMDOG_EXPORT EffectPassLoader final {
public:
	explicit EffectPassLoader(Details::AssetLoaderContext const& loaderContext);

	EffectPassLoader(EffectPassLoader &&);
	EffectPassLoader & operator=(EffectPassLoader &&);

	~EffectPassLoader();

	EffectPassLoader & VertexShaderGLSL(std::string const& filePath);

	EffectPassLoader & PixelShaderGLSL(std::string const& filePath);

	EffectPassLoader & VertexShaderHLSL(std::string const& filePath, std::string const& entryPoint);

	EffectPassLoader & PixelShaderHLSL(std::string const& filePath, std::string const& entryPoint);

	EffectPassLoader & InputElements(std::vector<VertexBufferBinding> const& inputElements);

	EffectPassLoader & InputElements(std::vector<VertexBufferBinding> && inputElements);

	std::shared_ptr<EffectPass> Load();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace AssetLoaders
}// namespace Pomdogs

#endif // !defined(POMDOG_EFFECTPASSLOADER_C9744EEA_2F5B_46D6_8880_B11D2DF37382_HPP)
