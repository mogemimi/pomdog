// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPRITEFONT_1C2133CC_E7F0_4FC2_A4D8_2D2862E4B3C6_HPP
#define POMDOG_SPRITEFONT_1C2133CC_E7F0_4FC2_A4D8_2D2862E4B3C6_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace Pomdog {
namespace Detail {
namespace SpriteFonts {

struct Glyph {
	Rectangle Subrect;

	///@note UTF-32
	std::uint32_t Character;
	std::int16_t XOffset;
	std::int16_t YOffset;
	std::int16_t XAdvance;
	std::uint8_t TexturePage;
};

}// namespace SpriteFonts
}// namespace Detail

class SpriteBatch;

///@~Japanese
/// @brief バッチレンダリング可能なスプライトフォントです。
class SpriteFont final {
public:
	SpriteFont(std::vector<std::shared_ptr<Texture2D>> && textures,
		std::vector<Detail::SpriteFonts::Glyph> const& glyphs,
		std::uint32_t defaultCharacter,
		std::int16_t spacing,
		std::int16_t lineSpacing);

	~SpriteFont();

	///@~Japanese
	/// @brief テキストの描画時の水平方向の長さと垂直方向の長さ（ピクセル単位）を取得します。
	/// @details 指定する文字列のエンコーディング形式は UTF-8 エンコーディングです。
	Vector2 MeasureString(char const* text) const;

	//Vector2 MeasureString(std::string const& text) const;

	///@~Japanese
	/// @brief デフォルト文字を取得します。
	/// @remarks 指定された文字のフォントが見つからない場合、デフォルト文字が表示されます。
	/// 文字列のエンコーディング形式は UTF-32 エンコーディングです。
	std::uint32_t DefaultCharacter() const;

	///@~Japanese
	/// @brief デフォルト文字を設定します。
	/// @remarks 指定された文字のフォントが見つからない場合、デフォルト文字が表示されます。
	/// 文字列のエンコーディング形式は UTF-32 エンコーディングです。
	void DefaultCharacter(std::uint32_t character);

	///@~Japanese
	/// @brief 行間を取得します。
	float LineSpacing() const;

	///@~Japanese
	/// @brief 行間を設定します。
	void LineSpacing(float lineSpacing);

	///@~Japanese
	/// @brief 指定した文字のフォントを含んでいるかどうか取得します。含んでいる場合は、true を返却します。
	bool ContainsCharacter(std::uint32_t character) const;

	void Begin(Matrix4x4 const& transformMatrix);

	///@~Japanese
	/// @brief 文字列をスプライトバッチを使って描画します。
	/// @remarks 文字列のエンコーディング形式は UTF-8 エンコーディングです。
	void Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color);

	///@~Japanese
	/// @brief 文字列をスプライトバッチを使って描画します。
	/// @remarks 文字列のエンコーディング形式は UTF-8 エンコーディングです。
	void Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, float scale, float layerDepth);

	///@~Japanese
	/// @brief 文字列をスプライトバッチを使って描画します。
	/// @remarks 文字列のエンコーディング形式は UTF-8 エンコーディングです。
	void Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);

	void End();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEFONT_1C2133CC_E7F0_4FC2_A4D8_2D2862E4B3C6_HPP)
