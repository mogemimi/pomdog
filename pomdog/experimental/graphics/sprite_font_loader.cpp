// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_font_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/texture_loader.h"
#include "pomdog/experimental/graphics/font_glyph.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <functional>
#include <regex>
#include <sstream>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

struct BitmapFontInfo final {
    std::string Face;
    i16 Size = 32;
    i16 StretchHeight = 0;
    i16 PaddingTop = 0;
    i16 PaddingBottom = 0;
    i16 PaddingLeft = 0;
    i16 PaddingRight = 0;
    i16 SpacingX = 0;
    i16 SpacingY = 0;
    bool Bold = false;
    bool Italic = false;
    bool Unicode = false;
    bool Smooth = true;
    bool AntiAlias = true;
};

struct BitmapFontCommon final {
    i16 LineHeight = 0;
    i16 Base = 0;
    i16 ScaleWidth = 0;
    i16 ScaleHeight = 0;
    i16 Pages = 0;
    bool Packed = false;
};

struct BitmapFontPage final {
    std::string Path;
    u16 ID;
};

//TEST(Regex, TrivialCase1)
//{
//    std::string source = "name=\"value\"";
//
//    auto r = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(\\\"([a-zA-Z0-9_\\-/.]*)\\\")");
//
//    EXPECT_TRUE(std::regex_match(source, r));
//    EXPECT_TRUE(std::regex_match("path=\"image.png\"", r));
//    EXPECT_TRUE(std::regex_match("key=\"file2014.png\"", r));
//    EXPECT_TRUE(std::regex_match("face=\"UbuntuMono-Regular\"", r));
//    EXPECT_TRUE(std::regex_match("face=\"usr/dev/UbuntuMono-Regular\"", r));
//    EXPECT_FALSE(std::regex_match("2=4", r));
//    EXPECT_FALSE(std::regex_match("key=\"file20\"14.png\"", r));
//    EXPECT_FALSE(std::regex_match("key=\"file20\"14.p\"ng\"", r));
//}
//
//TEST(Regex, TrivialCase2)
//{
//    std::string source = "name=42";
//
//    auto r = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)((\\-?[0-9]*)(,(\\-?[0-9]*)){0,3})");
//
//    EXPECT_TRUE(std::regex_match(source, r));
//    EXPECT_TRUE(std::regex_match("width=23", r));
//    EXPECT_TRUE(std::regex_match("width=-23", r));
//    EXPECT_TRUE(std::regex_match("padding=2,45", r));
//    EXPECT_TRUE(std::regex_match("padding=2,45,-4", r));
//    EXPECT_TRUE(std::regex_match("padding=-2,-45,-3,-2", r));
//
//    EXPECT_FALSE(std::regex_match("width=--23", r));
//    EXPECT_FALSE(std::regex_match("width=2-3", r));
//    EXPECT_FALSE(std::regex_match("width=+23", r));
//    EXPECT_FALSE(std::regex_match("padding=-2,-45,-3,-2,4", r));
//}

BitmapFontInfo ParseInfo(std::istream& stream)
{
    BitmapFontInfo info;

    std::string source;
    while (stream >> source && !stream.fail()) {
        auto expr = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(.*)");
        auto exprString = std::regex("\\\"([a-zA-Z0-9_\\-/.]*)\\\"");
        //auto exprNumber = std::regex("\\-?[0-9]*");
        auto exprVector2 = std::regex("(\\-?[0-9]*),(\\-?[0-9]*)");
        auto exprVector4 = std::regex("(\\-?[0-9]*),(\\-?[0-9]*),(\\-?[0-9]*),(\\-?[0-9]*)");

        std::smatch match;
        if (std::regex_match(source, match, expr)) {
            POMDOG_ASSERT(match.size() >= 4);
            auto name = match[1].str();
            auto arguments = match[3].str();

            std::smatch match2;

            if (name == "face") {
                if (std::regex_match(arguments, match2, exprString)) {
                    POMDOG_ASSERT(match2.size() >= 2);
                    info.Face = match2[1];
                }
            }
            else if (name == "size") {
                static_assert(std::is_same<decltype(info.Size), std::int16_t>::value, "");
                info.Size = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "bold") {
                info.Bold = std::stoul(arguments) != 0;
            }
            else if (name == "italic") {
                info.Italic = std::stoul(arguments) != 0;
            }
            // else if (name == "charset") {
            //     if (std::regex_match(arguments, match2, exprString)) {
            //         POMDOG_ASSERT(match2.size() >= 2);
            //         std::cout << name << ": " << match2[1] << std::endl;
            //     }
            // }
            else if (name == "unicode") {
                info.Unicode = std::stoul(arguments) != 0;
            }
            else if (name == "stretchH") {
                static_assert(std::is_same<decltype(info.StretchHeight), std::int16_t>::value, "");
                info.StretchHeight = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "smooth") {
                info.Smooth = std::stoul(arguments) != 0;
            }
            else if (name == "aa") {
                info.AntiAlias = std::stoul(arguments) != 0;
            }
            else if (name == "padding") {
                if (std::regex_match(arguments, match2, exprVector4)) {
                    static_assert(std::is_same<decltype(info.PaddingTop), std::int16_t>::value, "");
                    static_assert(std::is_same<decltype(info.PaddingRight), std::int16_t>::value, "");
                    static_assert(std::is_same<decltype(info.PaddingBottom), std::int16_t>::value, "");
                    static_assert(std::is_same<decltype(info.PaddingLeft), std::int16_t>::value, "");
                    POMDOG_ASSERT(match2.size() >= 5);
                    info.PaddingTop = static_cast<std::int16_t>(std::stoi(match2[1]));
                    info.PaddingRight = static_cast<std::int16_t>(std::stoi(match2[2]));
                    info.PaddingBottom = static_cast<std::int16_t>(std::stoi(match2[3]));
                    info.PaddingLeft = static_cast<std::int16_t>(std::stoi(match2[4]));
                }
            }
            else if (name == "spacing") {
                if (std::regex_match(arguments, match2, exprVector2)) {
                    static_assert(std::is_same<decltype(info.SpacingX), std::int16_t>::value, "");
                    static_assert(std::is_same<decltype(info.SpacingY), std::int16_t>::value, "");
                    POMDOG_ASSERT(match2.size() >= 3);
                    info.SpacingX = static_cast<std::int16_t>(std::stoi(match2[1]));
                    info.SpacingY = static_cast<std::int16_t>(std::stoi(match2[2]));
                }
            }
        }
    }

    return info;
}

BitmapFontCommon ParseCommon(std::istream& stream)
{
    BitmapFontCommon result;

    std::string source;
    while (stream >> source && !stream.fail()) {
        auto expr = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(.*)");
        std::smatch match;
        if (std::regex_match(source, match, expr)) {
            POMDOG_ASSERT(match.size() >= 4);
            auto name = match[1].str();
            auto arguments = match[3].str();

            if (name == "lineHeight") {
                static_assert(std::is_same<decltype(result.LineHeight), std::int16_t>::value, "");
                result.LineHeight = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "base") {
                static_assert(std::is_same<decltype(result.Base), std::int16_t>::value, "");
                result.Base = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "scaleW") {
                static_assert(std::is_same<decltype(result.ScaleWidth), std::int16_t>::value, "");
                result.ScaleWidth = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "scaleH") {
                static_assert(std::is_same<decltype(result.ScaleHeight), std::int16_t>::value, "");
                result.ScaleHeight = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "pages") {
                static_assert(std::is_same<decltype(result.Pages), std::int16_t>::value, "");
                result.Pages = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "packed") {
                result.Packed = std::stoul(arguments) != 0;
            }
        }
    }

    return result;
}

BitmapFontPage ParsePage(std::istream& stream)
{
    BitmapFontPage result;

    std::string source;
    while (stream >> source && !stream.fail()) {
        auto expr = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(.*)");
        auto exprString = std::regex("\\\"([a-zA-Z0-9_\\-/.]*)\\\"");
        std::smatch match;
        if (std::regex_match(source, match, expr)) {
            POMDOG_ASSERT(match.size() >= 4);
            auto name = match[1].str();
            auto arguments = match[3].str();

            std::smatch match2;

            if (name == "file") {
                if (std::regex_match(arguments, match2, exprString)) {
                    POMDOG_ASSERT(match2.size() >= 2);
                    result.Path = match2[1];
                }
            }
            else if (name == "id") {
                static_assert(std::is_same<decltype(result.ID), std::uint16_t>::value, "");
                result.ID = static_cast<std::uint16_t>(std::stoul(arguments));
            }
        }
    }

    return result;
}

FontGlyph ParseGlyph(std::istream& stream)
{
    FontGlyph result;
    result.subrect.x = 0;
    result.subrect.y = 0;
    result.subrect.width = 1;
    result.subrect.height = 1;
    result.character = 0;
    result.xOffset = 0;
    result.yOffset = 0;
    result.xAdvance = 0;
    result.texturePage = 0;

    std::string source;
    while (stream >> source && !stream.fail()) {
        auto expr = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(.*)");
        std::smatch match;
        if (std::regex_match(source, match, expr)) {
            POMDOG_ASSERT(match.size() >= 4);
            auto name = match[1].str();
            auto arguments = match[3].str();

            if (name == "id") {
                result.character = std::stoi(arguments);
            }
            else if (name == "page") {
                static_assert(std::is_same<decltype(result.texturePage), std::int16_t>::value, "");
                result.texturePage = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "x") {
                result.subrect.x = std::stoi(arguments);
            }
            else if (name == "y") {
                result.subrect.y = std::stoi(arguments);
            }
            else if (name == "width") {
                result.subrect.width = std::stoi(arguments);
            }
            else if (name == "height") {
                result.subrect.height = std::stoi(arguments);
            }
            else if (name == "xoffset") {
                static_assert(std::is_same<decltype(result.xOffset), std::int16_t>::value, "");
                result.xOffset = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "yoffset") {
                static_assert(std::is_same<decltype(result.yOffset), std::int16_t>::value, "");
                result.yOffset = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "xadvance") {
                static_assert(std::is_same<decltype(result.xAdvance), std::int16_t>::value, "");
                result.xAdvance = static_cast<std::int16_t>(std::stoi(arguments));
            }
        }
    }

    return result;
}

std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
parseSpriteFont(
    std::istream& stream,
    const std::string& filePath,
    const std::string& directoryName,
    const std::function<std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>(const std::string&)>& textureLoader)
{
    std::vector<BitmapFontPage> pages;
    std::vector<FontGlyph> glyphs;
    glyphs.reserve(127);

    BitmapFontInfo info;
    BitmapFontCommon common;

    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream ss(line);
        std::string objectName;
        ss >> objectName;
        if (objectName.empty()) {
            break;
        }

        if (objectName == "info") {
            info = ParseInfo(ss);
        }
        else if (objectName == "common") {
            common = ParseCommon(ss);
        }
        else if (objectName == "page") {
            pages.push_back(ParsePage(ss));
        }
        else if (objectName == "char") {
            glyphs.push_back(ParseGlyph(ss));
        }
    }

    if (pages.empty()) {
        auto err = errors::make("invalid file format " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }
    POMDOG_ASSERT(!pages.empty());

    auto pageLess = [](const BitmapFontPage& a, const BitmapFontPage& b) {
        return a.ID < b.ID;
    };

    std::sort(std::begin(pages), std::end(pages), pageLess);
    if (std::unique(std::begin(pages), std::end(pages), pageLess) != std::end(pages)) {
        auto err = errors::make("invalid file format " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }
    POMDOG_ASSERT(std::unique(std::begin(pages), std::end(pages), pageLess) == std::end(pages));

    POMDOG_ASSERT(pages.front().ID == 0);
    POMDOG_ASSERT(pages.back().ID == (pages.size() - 1));

    std::vector<std::shared_ptr<gpu::Texture2D>> textures;
    {
        for (auto& page : pages) {
            auto [texture, textureErr] = textureLoader(filepaths::joinUnix(directoryName, page.Path));
            if (textureErr != nullptr) {
                auto err = errors::wrap(std::move(textureErr), "failed to load sprite font texture " + page.Path);
                return std::make_tuple(nullptr, std::move(err));
            }
            textures.push_back(std::move(texture));
        }
    }

    POMDOG_ASSERT(!glyphs.empty());
    const auto defaultCharacter = glyphs.front().character;

    for (auto& glyph : glyphs) {
        glyph.yOffset = glyph.yOffset - common.Base;
    }

    auto spriteFont = std::make_shared<SpriteFont>(
        std::move(textures),
        std::move(glyphs),
        static_cast<std::int16_t>(info.PaddingLeft + info.PaddingRight),
        common.LineHeight);
    spriteFont->setDefaultCharacter(defaultCharacter);

    return std::make_tuple(std::move(spriteFont), nullptr);
}

} // namespace

std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
loadSpriteFont(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& filePath)
{
    auto [file, openErr] = vfs::open(fs, filePath);
    if (openErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "cannot open sprite font file, " + filePath));
    }

    auto [info, statErr] = file->stat();
    if (statErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(statErr), "cannot stat sprite font file, " + filePath));
    }

    std::vector<std::uint8_t> buffer(static_cast<std::size_t>(info.size));
    auto [bytesRead, readErr] = file->read(std::span<std::uint8_t>(buffer));
    if (readErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(readErr), "cannot read sprite font file, " + filePath));
    }

    std::istringstream stream(std::string(buffer.begin(), buffer.end()));
    auto directoryName = filepaths::getDirectoryName(filePath);

    return parseSpriteFont(stream, filePath, directoryName,
        [&fs, &graphicsDevice](const std::string& texturePath) {
            return loadTexture2D(fs, graphicsDevice, texturePath);
        });
}

} // namespace pomdog
