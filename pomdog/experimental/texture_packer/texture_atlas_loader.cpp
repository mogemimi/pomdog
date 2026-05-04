// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/texture_packer/texture_atlas_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/assert.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::TexturePacker {
namespace {

enum class ParserState : u8 {
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

    region.Region.subrectX = 0;
    region.Region.subrectY = 0;
    region.Region.subrectWidth = 1;
    region.Region.subrectHeight = 1;
    region.Region.width = 1;
    region.Region.height = 1;
    region.Region.xOffset = 0;
    region.Region.yOffset = 0;
    region.Region.rotate = false;

    return region;
}

std::tuple<TextureAtlas, std::unique_ptr<Error>>
parseTextureAtlas(std::istream& stream, const std::string& filePath)
{
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
                ss >> region.Region.subrectX >> region.Region.subrectY;
            }
            else if ("size" == propertyName) {
                ss >> region.Region.subrectWidth >> region.Region.subrectHeight;
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

        auto err = errors::make("cannot parse the file, " + filePath);
        return std::make_tuple(TextureAtlas{}, std::move(err));
    }

    return std::make_tuple(std::move(result), nullptr);
}

} // namespace

std::tuple<TextureAtlas, std::unique_ptr<Error>>
loadTextureAtlas(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath)
{
    auto [file, openErr] = vfs::open(fs, filePath);
    if (openErr != nullptr) {
        return std::make_tuple(TextureAtlas{}, errors::wrap(std::move(openErr), "cannot open atlas file, " + filePath));
    }

    auto [info, statErr] = file->stat();
    if (statErr != nullptr) {
        return std::make_tuple(TextureAtlas{}, errors::wrap(std::move(statErr), "cannot stat atlas file, " + filePath));
    }

    std::vector<std::uint8_t> buffer(static_cast<std::size_t>(info.size));
    auto [bytesRead, readErr] = file->read(std::span<std::uint8_t>(buffer));
    if (readErr != nullptr) {
        return std::make_tuple(TextureAtlas{}, errors::wrap(std::move(readErr), "cannot read atlas file, " + filePath));
    }

    std::istringstream stream(std::string(buffer.begin(), buffer.end()), std::ios::binary);
    return parseTextureAtlas(stream, filePath);
}

} // namespace pomdog::TexturePacker
