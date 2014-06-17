//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SpriteFontLoader.hpp"
#include <utility>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <regex>
#include <Pomdog/Content/detail/TextureLoader.hpp>

namespace Pomdog {
namespace Details {
//-----------------------------------------------------------------------
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
//	std::string source = "name=\"value\"";
//	
//	auto r = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)(\\\"([a-zA-Z0-9_\\-/.]*)\\\")");
//	
//	EXPECT_TRUE(std::regex_match(source, r));
//	EXPECT_TRUE(std::regex_match("path=\"image.png\"", r));
//	EXPECT_TRUE(std::regex_match("key=\"file2014.png\"", r));
//	EXPECT_TRUE(std::regex_match("face=\"UbuntuMono-Regular\"", r));
//	EXPECT_TRUE(std::regex_match("face=\"usr/dev/UbuntuMono-Regular\"", r));
//	EXPECT_FALSE(std::regex_match("2=4", r));
//	EXPECT_FALSE(std::regex_match("key=\"file20\"14.png\"", r));
//	EXPECT_FALSE(std::regex_match("key=\"file20\"14.p\"ng\"", r));
//}
//
//TEST(Regex, TrivialCase2)
//{
//	std::string source = "name=42";
//	
//	auto r = std::regex("([a-zA-Z_][a-zA-Z0-9_]*)(=)((\\-?[0-9]*)(,(\\-?[0-9]*)){0,3})");
//	
//	EXPECT_TRUE(std::regex_match(source, r));
//	EXPECT_TRUE(std::regex_match("width=23", r));
//	EXPECT_TRUE(std::regex_match("width=-23", r));
//	EXPECT_TRUE(std::regex_match("padding=2,45", r));
//	EXPECT_TRUE(std::regex_match("padding=2,45,-4", r));
//	EXPECT_TRUE(std::regex_match("padding=-2,-45,-3,-2", r));
//	
//	EXPECT_FALSE(std::regex_match("width=--23", r));
//	EXPECT_FALSE(std::regex_match("width=2-3", r));
//	EXPECT_FALSE(std::regex_match("width=+23", r));
//	EXPECT_FALSE(std::regex_match("padding=-2,-45,-3,-2,4", r));
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
				info.Size = std::stoul(arguments);
			}
			else if (name == "bold") {
				info.Bold = std::stoul(arguments) != 0;
			}
			else if (name == "italic") {
				info.Italic = std::stoul(arguments) != 0;
			}
//			else if (name == "charset") {
//				if (std::regex_match(arguments, match2, exprString)) {
//					POMDOG_ASSERT(match2.size() >= 2);
//					std::cout << name << ": " << match2[1] << std::endl;
//				}
//			}
			else if (name == "unicode") {
				info.Unicode = std::stoul(arguments) != 0;
			}
			else if (name == "stretchH") {
				info.StretchHeight = std::stoi(arguments);
			}
			else if (name == "smooth") {
				info.Smooth = std::stoul(arguments) != 0;
			}
			else if (name == "aa") {
				info.AntiAlias = std::stoul(arguments) != 0;
			}
			else if (name == "padding") {
				if (std::regex_match(arguments, match2, exprVector4)) {
					POMDOG_ASSERT(match2.size() >= 5);
					info.PaddingTop = std::stoi(match2[1]);
					info.PaddingRight = std::stoi(match2[2]);
					info.PaddingBottom = std::stoi(match2[3]);
					info.PaddingLeft = std::stoi(match2[4]);
				}
			}
			else if (name == "spacing") {
				if (std::regex_match(arguments, match2, exprVector2)) {
					POMDOG_ASSERT(match2.size() >= 3);
					info.SpacingX = std::stoi(match2[1]);
					info.SpacingY = std::stoi(match2[2]);
				}
			}
		}
	}
	
	return std::move(info);
}
//-----------------------------------------------------------------------
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
				result.LineHeight = std::stoul(arguments);
			}
			else if (name == "base") {
				result.Base = std::stoul(arguments);
			}
			else if (name == "scaleW") {
				result.ScaleWidth = std::stoul(arguments);
			}
			else if (name == "scaleH") {
				result.ScaleHeight = std::stoul(arguments);
			}
			else if (name == "pages") {
				result.Pages = std::stoul(arguments);
			}
			else if (name == "packed") {
				result.Packed = std::stoul(arguments) != 0;
			}
		}
	}
	
	return std::move(result);
}
//-----------------------------------------------------------------------
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
				result.Id = std::stoul(arguments);
			}
		}
	}
	
	return std::move(result);
}
//-----------------------------------------------------------------------
Details::SpriteFonts::Glyph ParseGlyph(std::istream & stream)
{
	Details::SpriteFonts::Glyph result;

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

			if (name == "id") {
				std::stringstream ss(arguments);
				ss >> result.Character;
			}
			else if (name == "page") {
				result.TexturePage = std::stoul(arguments);
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
				result.XOffset = std::stoi(arguments);
			}
			else if (name == "yoffset") {
				result.YOffset = std::stoi(arguments);
			}
			else if (name == "xadvance") {
				result.XAdvance = std::stoi(arguments);
			}
		}
	}
	
	return std::move(result);
}


namespace FilePathHelper {
	std::string Directory(std::string const& path)
	{
		std::string::size_type index(path.rfind("/"));
		return (std::string::npos != index) ?
			path.substr(0, ++index) : "";
	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
std::shared_ptr<SpriteFont>
AssetLoader<SpriteFont>::operator()(AssetLoaderContext const& loaderContext,
	std::string const& assetPath)
{
	auto filename = loaderContext.RootDirectory + "/" + assetPath;
	std::ifstream stream(filename, std::ios::binary);

	std::vector<BitmapFontPage> pages;
	std::vector<Details::SpriteFonts::Glyph> glyphs;
	glyphs.reserve(127);
	
	BitmapFontInfo info;
	BitmapFontCommon common;

	std::string line;
	while (std::getline(stream, line))
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
		///@todo Error
		/// Throw exception
	}
	POMDOG_ASSERT(!pages.empty());

	auto pageLess = [](BitmapFontPage const& a, BitmapFontPage const& b){
		return a.Id < b.Id;
	};

	std::sort(std::begin(pages), std::end(pages), pageLess);
	if (std::unique(std::begin(pages), std::end(pages), pageLess) != std::end(pages)) {
		///@todo Error
		/// Throw exception
	}
	POMDOG_ASSERT(std::unique(std::begin(pages), std::end(pages), pageLess) == std::end(pages));
	
	POMDOG_ASSERT(pages.front().Id == 0);
	POMDOG_ASSERT(pages.back().Id == (pages.size() - 1));

	std::vector<std::shared_ptr<Texture2D>> textures;
	{
		auto directoryName = FilePathHelper::Directory(assetPath);
		
		for (auto & page: pages)
		{
			textures.push_back(Details::AssetLoader<Texture2D>()(
				loaderContext, directoryName + page.Path));
		}
	}

	POMDOG_ASSERT(!glyphs.empty());
	std::uint32_t defaultCharacter = glyphs.front().Character;


	auto spriteFont = std::make_shared<SpriteFont>(std::move(textures), glyphs,
		defaultCharacter, info.PaddingLeft + info.PaddingRight, common.LineHeight);
	return std::move(spriteFont);
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
