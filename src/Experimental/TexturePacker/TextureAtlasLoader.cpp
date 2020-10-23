// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/TexturePacker/TextureAtlasLoader.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>

namespace Pomdog::TexturePacker {
namespace {

enum class ParserState {
    PageName,
    PageProperties,
    RegionName,
    RegionProperties,
    ParsingError,
};

TextureAtlasRegion CreateAtlasRegion(const std::string& line, std::int16_t pageIndex)
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

    return region;
}

} // namespace

std::tuple<TextureAtlas, std::shared_ptr<Error>>
TextureAtlasLoader::Load(const std::string& filePath)
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = Errors::New("cannot open the file, " + filePath);
        return std::make_tuple(TextureAtlas{}, std::move(err));
    }

    POMDOG_ASSERT(stream);

    TextureAtlas result;
    std::int16_t pageIndex = 0;

    ParserState state = ParserState::PageName;

    std::string line;
    while (std::getline(stream, line)) {
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
            auto& region = result.regions.back();

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
        } // end of switch-case

        if (state == ParserState::ParsingError) {
            break;
        }
    }

    if (state == ParserState::ParsingError) {
        result.pages.clear();
        result.regions.clear();

        auto err = Errors::New("cannot parse the file, " + filePath);
        return std::make_tuple(TextureAtlas{}, std::move(err));
    }

    return std::make_tuple(std::move(result), nullptr);
}

} // namespace Pomdog::TexturePacker
