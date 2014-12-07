//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AssetDictionary.hpp"

#include "AssetReaders/AudioBufferReader.hpp"
#include "AssetReaders/EffectPassReader.hpp"
#include "AssetReaders/Texture2DReader.hpp"

namespace Pomdog {
namespace Details {

class AssetReader {
public:
	virtual ~AssetReader() = default;

	virtual Any Read(AssetLoaderContext const& loaderContext, std::string const& assetName) = 0;
};

template <class ReaderFunction>
class AssetReaderDerived final: public AssetReader {
public:
	Any Read(AssetLoaderContext const& loaderContext, std::string const& assetName) override
	{
		return ReaderFunction::Read(loaderContext, assetName);
	}
};

//-----------------------------------------------------------------------
AssetDictionary::AssetDictionary()
{
	readers.emplace(typeid(std::shared_ptr<AudioBuffer>), std::make_unique<AssetReaderDerived<AudioBufferReader>>());
	readers.emplace(typeid(std::shared_ptr<EffectPass>), std::make_unique<AssetReaderDerived<EffectPassReader>>());
	readers.emplace(typeid(std::shared_ptr<Texture2D>), std::make_unique<AssetReaderDerived<Texture2DReader>>());
}
//-----------------------------------------------------------------------
AssetDictionary::~AssetDictionary() = default;
//-----------------------------------------------------------------------
void AssetDictionary::Unload()
{
	assets.clear();
}
//-----------------------------------------------------------------------
Any AssetDictionary::ReadAsset(AssetLoaderContext const& loaderContext,
	std::string const& assetName, std::type_index const& typeIndex) const
{
	auto iter = readers.find(typeIndex);
	
	POMDOG_ASSERT(iter != std::end(readers));
	
	if (iter == std::end(readers)) {
		/// FUS RO DAH!
		///@todo Not implemented
		//throw exception
	}
	
	auto & reader = iter->second;
	
	POMDOG_ASSERT(reader);
	return reader->Read(loaderContext, assetName);
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
