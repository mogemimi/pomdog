//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TEXTURE2D_F041ED3F_9089_4056_BC4F_1C145612778F_HPP
#define POMDOG_TEXTURE2D_F041ED3F_9089_4056_BC4F_1C145612778F_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "../Config/Export.hpp"
#include "../Config/FundamentalTypes.hpp"
#include "detail/ForwardDeclarations.hpp"
#include "SurfaceFormat.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeTexture2D;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 2 次元テクスチャです。
class POMDOG_EXPORT Texture2D //: public Texture
{
public:
	Texture2D() = delete;
	Texture2D(Texture2D const&) = delete;
	Texture2D(Texture2D &&) = default;

	Texture2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::uint32_t width, std::uint32_t height);

	~Texture2D();
	
	Texture2D & operator=(Texture2D const&) = delete;
	Texture2D & operator=(Texture2D &&) = default;

	///@~Japanese
	/// @brief テクスチャの水平方向の幅（ピクセル単位）を取得します。
	std::uint32_t GetWidth() const;

	///@~Japanese
	/// @brief テクスチャの垂直方向の幅（ピクセル単位）を取得します。
	std::uint32_t GetHeight() const;

	///@~Japanese
	/// @brief テクスチャのフォーマットを取得します。
	SurfaceFormat GetFormat() const;

	///@~Japanese
	/// @brief ミップマップレベルを取得します。
	/// @remarks ミップマップレベルは 1 以上を有効とします。
	std::uint32_t GetLevelCount() const;
	
public:
	Details::RenderSystem::NativeTexture2D* GetNativeTexture2D();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeTexture2D> nativeTexture2D;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_TEXTURE2D_F041ED3F_9089_4056_BC4F_1C145612778F_HPP)
