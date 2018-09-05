// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Graphics/SpriteFontLoader.hpp"
#include "Pomdog/Experimental/Graphics/FontGlyph.hpp"
#include "Pomdog/Experimental/Graphics/SpriteFont.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <utility>
#include <fstream>
#include <algorithm>
#include <regex>

namespace Pomdog {

namespace {

struct BitmapFontInfo {
    std::string Face;
    std::uint16_t Size = 32;
    std::uint16_t StretchHeight = 0;
    std::uint16_t PaddingTop = 0;
    std::uint16_t PaddingBottom = 0;
    std::uint16_t PaddingLeft = 0;
    std::uint16_t PaddingRight = 0;
    std::uint16_t SpacingX = 0;
    std::uint16_t SpacingY = 0;
    bool Bold = false;
    bool Italic = false;
    bool Unicode = false;
    bool Smooth = true;
    bool AntiAlias = true;
};

struct BitmapFontCommon {
    std::uint16_t LineHeight;
    std::uint16_t Base;
    std::uint16_t ScaleWidth;
    std::uint16_t ScaleHeight;
    std::uint16_t Pages;
    bool Packed = false;
};

struct BitmapFontPage {
    std::string Path;
    std::uint16_t Id;
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

BitmapFontInfo ParseInfo(std::istream & stream)
{
    BitmapFontInfo info;

    std::string source;
    while (stream >> source && !stream.fail())
    {
        auto expr = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(.*)");
        auto exprString = std::regex("\\\"([a-zA-Z0-9_\\-/.]*)\\\"");
        //auto exprNumber = std::regex("\\-?[0-9]*");
        auto exprVector2 = std::regex("(\\-?[0-9]*),(\\-?[0-9]*)");
        auto exprVector4 = std::regex("(\\-?[0-9]*),(\\-?[0-9]*),(\\-?[0-9]*),(\\-?[0-9]*)");

        std::smatch match;
        if (std::regex_match(source, match, expr))
        {
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
                static_assert(std::is_same<decltype(info.Size), std::uint16_t>::value, "");
                info.Size = static_cast<std::uint16_t>(std::stoi(arguments));
            }
            else if (name == "bold") {
                info.Bold = std::stoul(arguments) != 0;
            }
            else if (name == "italic") {
                info.Italic = std::stoul(arguments) != 0;
            }
//            else if (name == "charset") {
//                if (std::regex_match(arguments, match2, exprString)) {
//                    POMDOG_ASSERT(match2.size() >= 2);
//                    std::cout << name << ": " << match2[1] << std::endl;
//                }
//            }
            else if (name == "unicode") {
                info.Unicode = std::stoul(arguments) != 0;
            }
            else if (name == "stretchH") {
                static_assert(std::is_same<decltype(info.StretchHeight), std::uint16_t>::value, "");
                info.StretchHeight = static_cast<std::uint16_t>(std::stoi(arguments));
            }
            else if (name == "smooth") {
                info.Smooth = std::stoul(arguments) != 0;
            }
            else if (name == "aa") {
                info.AntiAlias = std::stoul(arguments) != 0;
            }
            else if (name == "padding") {
                if (std::regex_match(arguments, match2, exprVector4)) {
                    static_assert(std::is_same<decltype(info.PaddingTop), std::uint16_t>::value, "");
                    static_assert(std::is_same<decltype(info.PaddingRight), std::uint16_t>::value, "");
                    static_assert(std::is_same<decltype(info.PaddingBottom), std::uint16_t>::value, "");
                    static_assert(std::is_same<decltype(info.PaddingLeft), std::uint16_t>::value, "");
                    POMDOG_ASSERT(match2.size() >= 5);
                    info.PaddingTop = static_cast<std::uint16_t>(std::stoi(match2[1]));
                    info.PaddingRight = static_cast<std::uint16_t>(std::stoi(match2[2]));
                    info.PaddingBottom = static_cast<std::uint16_t>(std::stoi(match2[3]));
                    info.PaddingLeft = static_cast<std::uint16_t>(std::stoi(match2[4]));
                }
            }
            else if (name == "spacing") {
                if (std::regex_match(arguments, match2, exprVector2)) {
                    static_assert(std::is_same<decltype(info.SpacingX), std::uint16_t>::value, "");
                    static_assert(std::is_same<decltype(info.SpacingY), std::uint16_t>::value, "");
                    POMDOG_ASSERT(match2.size() >= 3);
                    info.SpacingX = static_cast<std::uint16_t>(std::stoi(match2[1]));
                    info.SpacingY = static_cast<std::uint16_t>(std::stoi(match2[2]));
                }
            }
        }
    }

    return info;
}

BitmapFontCommon ParseCommon(std::istream & stream)
{
    BitmapFontCommon result;

    std::string source;
    while (stream >> source && !stream.fail())
    {
        auto expr = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(.*)");
        std::smatch match;
        if (std::regex_match(source, match, expr))
        {
            POMDOG_ASSERT(match.size() >= 4);
            auto name = match[1].str();
            auto arguments = match[3].str();

            if (name == "lineHeight") {
                static_assert(std::is_same<decltype(result.LineHeight), std::uint16_t>::value, "");
                result.LineHeight = static_cast<std::uint16_t>(std::stoi(arguments));
            }
            else if (name == "base") {
                static_assert(std::is_same<decltype(result.Base), std::uint16_t>::value, "");
                result.Base = static_cast<std::uint16_t>(std::stoi(arguments));
            }
            else if (name == "scaleW") {
                static_assert(std::is_same<decltype(result.ScaleWidth), std::uint16_t>::value, "");
                result.ScaleWidth = static_cast<std::uint16_t>(std::stoi(arguments));
            }
            else if (name == "scaleH") {
                static_assert(std::is_same<decltype(result.ScaleHeight), std::uint16_t>::value, "");
                result.ScaleHeight = static_cast<std::uint16_t>(std::stoi(arguments));
            }
            else if (name == "pages") {
                static_assert(std::is_same<decltype(result.Pages), std::uint16_t>::value, "");
                result.Pages = static_cast<std::uint16_t>(std::stoi(arguments));
            }
            else if (name == "packed") {
                result.Packed = std::stoul(arguments) != 0;
            }
        }
    }

    return result;
}

BitmapFontPage ParsePage(std::istream & stream)
{
    BitmapFontPage result;

    std::string source;
    while (stream >> source && !stream.fail())
    {
        auto expr = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(.*)");
        auto exprString = std::regex("\\\"([a-zA-Z0-9_\\-/.]*)\\\"");
        std::smatch match;
        if (std::regex_match(source, match, expr))
        {
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
                static_assert(std::is_same<decltype(result.Id), std::uint16_t>::value, "");
                result.Id = static_cast<std::uint16_t>(std::stoul(arguments));
            }
        }
    }

    return result;
}

FontGlyph ParseGlyph(std::istream & stream)
{
    FontGlyph result;
    result.Subrect.X = 0;
    result.Subrect.Y = 0;
    result.Subrect.Width = 1;
    result.Subrect.Height = 1;
    result.Character = 0;
    result.XOffset = 0;
    result.YOffset = 0;
    result.XAdvance = 0;
    result.TexturePage = 0;

    std::string source;
    while (stream >> source && !stream.fail()) {
        auto expr = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(.*)");
        std::smatch match;
        if (std::regex_match(source, match, expr)) {
            POMDOG_ASSERT(match.size() >= 4);
            auto name = match[1].str();
            auto arguments = match[3].str();

            if (name == "id") {
                result.Character = std::stoi(arguments);
            }
            else if (name == "page") {
                static_assert(std::is_same<decltype(result.TexturePage), std::int16_t>::value, "");
                result.TexturePage = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "x") {
                result.Subrect.X = std::stoi(arguments);
            }
            else if (name == "y") {
                result.Subrect.Y = std::stoi(arguments);
            }
            else if (name == "width") {
                result.Subrect.Width = std::stoi(arguments);
            }
            else if (name == "height") {
                result.Subrect.Height = std::stoi(arguments);
            }
            else if (name == "xoffset") {
                static_assert(std::is_same<decltype(result.XOffset), std::int16_t>::value, "");
                result.XOffset = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "yoffset") {
                static_assert(std::is_same<decltype(result.YOffset), std::int16_t>::value, "");
                result.YOffset = static_cast<std::int16_t>(std::stoi(arguments));
            }
            else if (name == "xadvance") {
                static_assert(std::is_same<decltype(result.XAdvance), std::int16_t>::value, "");
                result.XAdvance = static_cast<std::int16_t>(std::stoi(arguments));
            }
        }
    }

    return result;
}

} // unnamed namespace

std::shared_ptr<SpriteFont> SpriteFontLoader::Load(
    AssetManager & assets, const std::string& assetName)
{
    auto binaryFile = assets.OpenStream(assetName);

    if (!binaryFile.Stream) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file");
    }

    if (binaryFile.SizeInBytes <= 0) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The file is too small");
    }

    std::vector<BitmapFontPage> pages;
    std::vector<FontGlyph> glyphs;
    glyphs.reserve(127);

    BitmapFontInfo info;
    BitmapFontCommon common;

    std::string line;
    while (std::getline(binaryFile.Stream, line))
    {
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
        //else if (objectName == "chars") {
        //}
        else if (objectName == "char") {
            glyphs.push_back(ParseGlyph(ss));
        }
    }

    if (pages.empty()) {
        // FUS RO DAH
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid file format");
    }
    POMDOG_ASSERT(!pages.empty());

    auto pageLess = [](const BitmapFontPage& a, const BitmapFontPage& b) {
        return a.Id < b.Id;
    };

    std::sort(std::begin(pages), std::end(pages), pageLess);
    if (std::unique(std::begin(pages), std::end(pages), pageLess) != std::end(pages)) {
        // FUS RO DAH
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid file format");
    }
    POMDOG_ASSERT(std::unique(std::begin(pages), std::end(pages), pageLess) == std::end(pages));

    POMDOG_ASSERT(pages.front().Id == 0);
    POMDOG_ASSERT(pages.back().Id == (pages.size() - 1));

    std::vector<std::shared_ptr<Texture2D>> textures;
    {
        auto directoryName = std::get<0>(PathHelper::Split(assetName));
        for (auto & page: pages) {
            textures.push_back(assets.Load<Texture2D>(directoryName + page.Path));
        }
    }

    POMDOG_ASSERT(!glyphs.empty());
    auto defaultCharacter = glyphs.front().Character;

    // FIXME: Replace the types of the following variables with signed 16-bit integer (std::int16_t).
    static_assert(std::is_same<decltype(info.PaddingLeft), std::uint16_t>::value, "");
    static_assert(std::is_same<decltype(info.PaddingRight), std::uint16_t>::value, "");
    static_assert(std::is_same<decltype(common.LineHeight), std::uint16_t>::value, "");

    auto spriteFont = std::make_shared<SpriteFont>(
        std::move(textures),
        std::move(glyphs),
        static_cast<std::int16_t>(info.PaddingLeft + info.PaddingRight),
        static_cast<std::int16_t>(common.LineHeight));
    spriteFont->SetDefaultCharacter(defaultCharacter);

    return spriteFont;
}

} // namespace Pomdog
