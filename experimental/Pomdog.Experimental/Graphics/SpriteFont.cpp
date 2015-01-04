//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "SpriteFont.hpp"
#include "SpriteBatch.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <unordered_map>
#include <codecvt>
#include <locale>

namespace Pomdog {
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - SpriteFont::Impl
#endif
//-----------------------------------------------------------------------
class SpriteFont::Impl {
public:
	typedef Details::SpriteFonts::Glyph Glyph;

	std::unordered_map<std::uint32_t, Glyph> spriteFontMap;

	Matrix4x4 transformMatrix;

	std::uint32_t defaultCharacter;
	std::uint16_t lineSpacing;
	std::uint16_t spacing;

	Impl(std::vector<std::shared_ptr<Texture2D>> && textures,
		std::vector<Details::SpriteFonts::Glyph> const& glyphs,
		std::uint32_t defaultCharacter, std::int16_t spacing, std::int16_t lineSpacing);

	Vector2 MeasureString(std::u32string const& text) const;

	void Draw(SpriteBatch & spriteBatch, std::u32string const& text, Vector2 const& position, Color const& color);
	
	void Draw(SpriteBatch & spriteBatch, std::u32string const& text, Vector2 const& position, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);

private:
	std::vector<std::shared_ptr<Texture2D>> textures;
};
//-----------------------------------------------------------------------
SpriteFont::Impl::Impl(std::vector<std::shared_ptr<Texture2D>> && texturesIn,
	std::vector<Details::SpriteFonts::Glyph> const& glyphsIn,
	std::uint32_t defaultCharacterIn, std::int16_t spacingIn, std::int16_t lineSpacingIn)
	: textures(std::move(texturesIn))
	, defaultCharacter(defaultCharacterIn)
	, spacing(spacingIn)
	, lineSpacing(lineSpacingIn)
	, transformMatrix(Matrix4x4::Identity)
{
	for (auto & glyph: glyphsIn) {
		spriteFontMap.emplace(glyph.Character, glyph);
	}
}
//-----------------------------------------------------------------------
Vector2 SpriteFont::Impl::MeasureString(std::u32string const& text) const
{
	POMDOG_ASSERT(!textures.empty());

	Vector2 result = Vector2::Zero;
	Vector2 currentPosition = Vector2::Zero;

	for (auto & character: text)
	{
		if (character == U'\n')
		{
			currentPosition.X = 0.0f;
			currentPosition.Y += lineSpacing;
			continue;
		}

		auto iter = spriteFontMap.find(character);
		if (iter == std::end(spriteFontMap))
		{
			iter = spriteFontMap.find(defaultCharacter);
			POMDOG_ASSERT(iter != std::end(spriteFontMap));
		}

		POMDOG_ASSERT(iter != std::end(spriteFontMap));
		auto const & glyph = iter->second;

		currentPosition.X += (glyph.XAdvance - spacing);
		
		result.X = std::max(result.X, currentPosition.X);
		result.Y = std::max(result.Y, currentPosition.Y);
	}
	
	return std::move(result);
}
//-----------------------------------------------------------------------
void SpriteFont::Impl::Draw(SpriteBatch & spriteBatch,
	std::u32string const& text, Vector2 const& position, Color const& color)
{
	if (text.empty()) {
		return;
	}

	POMDOG_ASSERT(!textures.empty());

	spriteBatch.Begin(SpriteSortMode::Deferred, transformMatrix);

	Vector2 currentPosition = position;

	for (auto & character: text)
	{
		if (character == U'\n')
		{
			currentPosition.X = position.X;
			currentPosition.Y += lineSpacing;
			continue;
		}

		auto iter = spriteFontMap.find(character);
		if (iter == std::end(spriteFontMap))
		{
			iter = spriteFontMap.find(defaultCharacter);
			POMDOG_ASSERT(iter != std::end(spriteFontMap));
		}

		POMDOG_ASSERT(iter != std::end(spriteFontMap));
		auto const & glyph = iter->second;

		if (glyph.Subrect.Width > 0 && glyph.Subrect.Height > 0)
		{
			POMDOG_ASSERT(glyph.TexturePage < textures.size());

			spriteBatch.Draw(textures[glyph.TexturePage],
				currentPosition + Vector2(glyph.XOffset, glyph.YOffset),
				glyph.Subrect, color, 0.0f, {0.0f, 0.0f}, 1.0f, 0.0f);
		}
		
		currentPosition.X += (glyph.XAdvance - spacing);
	}
	
	spriteBatch.End();
}
//-----------------------------------------------------------------------
void SpriteFont::Impl::Draw(SpriteBatch & spriteBatch,
	std::u32string const& text, Vector2 const& position, Color const& color,
	Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth)
{
	if (text.empty()) {
		return;
	}

	POMDOG_ASSERT(!textures.empty());
	
	spriteBatch.Begin(SpriteSortMode::Deferred, Matrix4x4::CreateRotationZ(rotation)
		* Matrix4x4::CreateScale({scale, 1.0f})
		* Matrix4x4::CreateTranslation({position, 0.0f})
		* transformMatrix);

	Vector2 currentPosition = Vector2::Zero;

	for (auto & character: text)
	{
		if (character == U'\n')
		{
			currentPosition.X = 0.0f;
			currentPosition.Y += lineSpacing;
			continue;
		}

		auto iter = spriteFontMap.find(character);
		if (iter == std::end(spriteFontMap))
		{
			iter = spriteFontMap.find(defaultCharacter);
			POMDOG_ASSERT(iter != std::end(spriteFontMap));
		}

		POMDOG_ASSERT(iter != std::end(spriteFontMap));
		auto const & glyph = iter->second;

		if (glyph.Subrect.Width > 0 && glyph.Subrect.Height > 0)
		{
			POMDOG_ASSERT(glyph.TexturePage < textures.size());

			spriteBatch.Draw(textures[glyph.TexturePage],
				currentPosition + Vector2(glyph.XOffset, glyph.YOffset),
				glyph.Subrect, color, 0.0f, {0.0f, 0.0f}, 1.0f, layerDepth);
		}
		
		currentPosition.X += (glyph.XAdvance - spacing);
	}
	
	spriteBatch.End();
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - SpriteFont
#endif
//-----------------------------------------------------------------------
SpriteFont::SpriteFont(std::vector<std::shared_ptr<Texture2D>> && textures,
	std::vector<Details::SpriteFonts::Glyph> const& glyphs,
	std::uint32_t defaultCharacter, std::int16_t spacing, std::int16_t lineSpacing)
	: impl(std::make_unique<Impl>(std::move(textures), glyphs, defaultCharacter, spacing, lineSpacing))
{
}
//-----------------------------------------------------------------------
SpriteFont::~SpriteFont() = default;
//-----------------------------------------------------------------------
Vector2 SpriteFont::MeasureString(char const* text) const
{
	POMDOG_ASSERT(text != nullptr);

	std::string utf8String(text);

	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8ToUtf32Conv;
	try {
		std::u32string utf32String = utf8ToUtf32Conv.from_bytes(utf8String);
		return impl->MeasureString(utf32String);
	}
	catch (std::exception const&) {
	}
	return Vector2::Zero;
}
//-----------------------------------------------------------------------
std::uint32_t SpriteFont::DefaultCharacter() const
{
	POMDOG_ASSERT(impl);
	return impl->defaultCharacter;
}
//-----------------------------------------------------------------------
void SpriteFont::DefaultCharacter(std::uint32_t character)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(ContainsCharacter(character));
	impl->defaultCharacter = character;
}
//-----------------------------------------------------------------------
float SpriteFont::LineSpacing() const
{
	POMDOG_ASSERT(impl);
	return impl->lineSpacing;
}
//-----------------------------------------------------------------------
void SpriteFont::LineSpacing(float lineSpacingIn)
{
	POMDOG_ASSERT(impl);
	impl->lineSpacing = lineSpacingIn;
}
//-----------------------------------------------------------------------
bool SpriteFont::ContainsCharacter(std::uint32_t character) const
{
	POMDOG_ASSERT(impl);
	return impl->spriteFontMap.find(character) != std::end(impl->spriteFontMap);
}
//-----------------------------------------------------------------------
void SpriteFont::Begin(Matrix4x4 const& transformMatrix)
{
	POMDOG_ASSERT(impl);
	impl->transformMatrix = transformMatrix;
}
//-----------------------------------------------------------------------
void SpriteFont::Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8ToUtf32Conv;
	try {
		std::u32string utf32String = utf8ToUtf32Conv.from_bytes(text);
		impl->Draw(spriteBatch, utf32String, position, color);
	}
	catch (std::exception const&) {
	}
}
//-----------------------------------------------------------------------
void SpriteFont::Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color,
	Radian<float> const& rotation, Vector2 const& originPivot, float scale, float layerDepth)
{
	this->Draw(spriteBatch, text, position, color, rotation, originPivot, Vector2{scale, scale}, layerDepth);
}
//-----------------------------------------------------------------------
void SpriteFont::Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color,
	Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth)
{
	//POMDOG_ASSERT_MESSAGE(text.empty(), "Not implemented");

	///@todo Not implemented
	
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8ToUtf32Conv;
	try {
		std::u32string utf32String = utf8ToUtf32Conv.from_bytes(text);
		impl->Draw(spriteBatch, utf32String, position, color,
			rotation, originPivot, scale, layerDepth);
	}
	catch (std::exception const&) {
	}
}
//-----------------------------------------------------------------------
void SpriteFont::End()
{
}

}// namespace Pomdog
