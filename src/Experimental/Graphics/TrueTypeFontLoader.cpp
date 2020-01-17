// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Graphics/TrueTypeFontLoader.hpp"
#include "Pomdog/Experimental/Graphics/TrueTypeFont.hpp"
#include <utility>

namespace Pomdog::Detail {

std::tuple<std::shared_ptr<TrueTypeFont>, std::shared_ptr<Error>>
AssetLoader<TrueTypeFont>::operator()([[maybe_unused]] AssetManager& assets, const std::string& filePath)
{
    auto font = std::make_shared<TrueTypeFont>();

    if (auto err = font->Load(filePath); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }

    return std::make_tuple(std::move(font), nullptr);
}

} // namespace Pomdog::Detail
