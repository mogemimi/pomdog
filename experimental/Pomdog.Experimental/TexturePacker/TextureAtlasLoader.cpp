// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "TextureAtlasLoader.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>
#include <fstream>
#include <algorithm>
#include <sstream>

namespace Pomdog {
namespace TexturePacker {
namespace {

enum class ParserState {
    PageName,
    PageProperties,
    RegionName,
    RegionProperties,
    ParsingError,
};

TextureAtlasRegion CreateAtlasRegion(std::string const& line, std::int16_t pageIndex)
{
    TextureAtlasRegion region;
    region.Name = line;
    region.TexturePage = pageIndex;

    region.Region.Subrect = {0, 0, 1, 1};
    region.Region.Width = 1;
    region.Region.Height = 1;
    region.Region.XOffset = 0;
    region.Region.YOffset = 0;
    region.Region.Rotate = false;

    return std::move(region);
}

} // unnamed namespace

TextureAtlas TextureAtlasLoader::Load(AssetManager const& assets, std::string const& assetName)
{
    auto binaryFile = assets.OpenStream(assetName);

    if (!binaryFile.Stream) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file");
    }

    TextureAtlas result;
    std::int16_t pageIndex = 0;

    ParserState state = ParserState::PageName;

    std::string line;
    while (std::getline(binaryFile.Stream, line)) {
        switch (state) {
        case ParserState::ParsingError: {
            break;
        }
        case ParserState::PageName: {
            if (line.empty()) {
                break;
            }

            TextureAtlasPage page;
            page.Name = line;
            result.pages.push_back(page);
            POMDOG_ASSERT((pageIndex + 1) == static_cast<int>(result.pages.size()));

            state = ParserState::PageProperties;
            break;
        }
        case ParserState::PageProperties: {
            if (line.empty()) {
                break;
            }
            std::istringstream ss(line);
            std::string propertyName;
            std::string propertyValue;
            ss >> propertyName >> propertyValue;
            if (propertyName.empty()) {
                state = ParserState::ParsingError;
                break;
            }
            if (ss) {
                // TODO
            }
            else {
                POMDOG_ASSERT(!ss);
                //state = ParserState::RegionName;
                result.regions.push_back(CreateAtlasRegion(line, pageIndex));
                state = ParserState::RegionProperties;
            }
            break;
        }
        case ParserState::RegionName: {
            break;
        }
        case ParserState::RegionProperties: {
            POMDOG_ASSERT(!result.regions.empty());

            if (line.empty()) {
                state = ParserState::PageName;
                ++pageIndex;
                break;
            }
            else if (line.front() != ' ') {
                //state = ParserState::RegionName;
                result.regions.push_back(CreateAtlasRegion(line, pageIndex));
                state = ParserState::RegionProperties;
                break;
            }

            std::replace(std::begin(line), std::end(line), ':', ' ');
            std::replace(std::begin(line), std::end(line), ',', ' ');

            std::istringstream ss(line);
            std::string propertyName;
            ss >> propertyName;

            if (propertyName.empty()) {
                state = ParserState::ParsingError;
                break;
            }

            POMDOG_ASSERT(!result.regions.empty());
            auto & region = result.regions.back();

            if ("rotate" == propertyName) {
                std::string value;
                ss >> value;
                if (value.empty()) {
                    state = ParserState::ParsingError;
                }
                else if (value.front() == 't') {
                    region.Region.Rotate = true;
                }
                else if (value.front() == 'f') {
                    region.Region.Rotate = false;
                }
            }
            else if ("xy" == propertyName) {
                ss >> region.Region.Subrect.X >> region.Region.Subrect.Y;
            }
            else if ("size" == propertyName) {
                ss >> region.Region.Subrect.Width >> region.Region.Subrect.Height;
            }
            else if ("orig" == propertyName) {
                ss >> region.Region.Width >> region.Region.Height;
            }
            else if ("offset" == propertyName) {
                ss >> region.Region.XOffset >> region.Region.YOffset;
            }
            //else if ("index" == propertyName) {
            //    // TODO
            //}

            if (!ss) {
                state = ParserState::ParsingError;
            }
            break;
        }
        } // switch (state)

        if (state == ParserState::ParsingError) {
            break;
        }
    }

    if (state == ParserState::ParsingError) {
        result.pages.clear();
        result.regions.clear();
    }

    return std::move(result);
}

} // namespace TexturePacker
} // namespace Pomdog
