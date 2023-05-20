// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/texture_packer/texture_atlas_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::TexturePacker {
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

    region.Region.subrect = {0, 0, 1, 1};
    region.Region.width = 1;
    region.Region.height = 1;
    region.Region.xOffset = 0;
    region.Region.yOffset = 0;
    region.Region.rotate = false;

    return region;
}

} // namespace

std::tuple<TextureAtlas, std::unique_ptr<Error>>
TextureAtlasLoader::Load(const std::string& filePath)
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = errors::New("cannot open the file, " + filePath);
        return std::make_tuple(TextureAtlas{}, std::move(err));
    }

    POMDOG_ASSERT(stream);

    TextureAtlas result;
    std::int16_t pageIndex = 0;

    ParserState state = ParserState::PageName;

    std::string line;
    while (std::getline(stream, line)) {
        while (!line.empty() && (line.back() == '\r')) {
            line.pop_back();
        }

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
                    region.Region.rotate = true;
                }
                else if (value.front() == 'f') {
                    region.Region.rotate = false;
                }
            }
            else if ("xy" == propertyName) {
                ss >> region.Region.subrect.x >> region.Region.subrect.y;
            }
            else if ("size" == propertyName) {
                ss >> region.Region.subrect.width >> region.Region.subrect.height;
            }
            else if ("orig" == propertyName) {
                ss >> region.Region.width >> region.Region.height;
            }
            else if ("offset" == propertyName) {
                ss >> region.Region.xOffset >> region.Region.yOffset;
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

        auto err = errors::New("cannot parse the file, " + filePath);
        return std::make_tuple(TextureAtlas{}, std::move(err));
    }

    return std::make_tuple(std::move(result), nullptr);
}

} // namespace pomdog::TexturePacker
