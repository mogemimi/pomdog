//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_RENDERTARGET2D_65AA5E90_1779_4953_8BD3_0B0833D5BF36_HPP
#define POMDOG_RENDERTARGET2D_65AA5E90_1779_4953_8BD3_0B0833D5BF36_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "Texture.hpp"
#include "DepthFormat.hpp"
#include "SurfaceFormat.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Config/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeRenderTarget2D;

}// namespace RenderSystem
}// namespace Details

///@~Japanese
/// @brief 深度バッファを含む、2 次元のレンダーターゲットです。
class POMDOG_EXPORT RenderTarget2D : public Texture {
public:
	RenderTarget2D() = delete;
	RenderTarget2D(RenderTarget2D const&) = delete;
	RenderTarget2D(RenderTarget2D &&) = default;

	RenderTarget2D(GraphicsDevice & graphicsDevice,
		std::int32_t width, std::int32_t height);

	RenderTarget2D(GraphicsDevice & graphicsDevice,
		std::int32_t width, std::int32_t height, bool generateMipmap,
		SurfaceFormat format, DepthFormat depthStencilFormat);

	RenderTarget2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::int32_t width, std::int32_t height);

	RenderTarget2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::int32_t width, std::int32_t height, bool generateMipmap,
		SurfaceFormat format, DepthFormat depthStencilFormat);

	~RenderTarget2D();
	
	RenderTarget2D & operator=(RenderTarget2D const&) = delete;
	RenderTarget2D & operator=(RenderTarget2D &&) = default;

	///@~Japanese
	/// @brief テクスチャの水平方向の幅（ピクセル単位）を取得します。
	std::int32_t Width() const;

	///@~Japanese
	/// @brief テクスチャの垂直方向の幅（ピクセル単位）を取得します。
	std::int32_t Height() const;

	///@~Japanese
	/// @brief ミップマップレベルを取得します。
	/// @remarks ミップマップレベルは 1 以上を有効とします。
	std::uint32_t LevelCount() const;

	///@~Japanese
	/// @brief テクスチャのフォーマットを取得します。
	SurfaceFormat Format() const;

	///@~Japanese
	/// @brief 深度バッファのフォーマットを取得します。
	DepthFormat DepthStencilFormat() const;

	///@~Japanese
	/// @brief レンダーターゲットを表す矩形を取得します。
	Rectangle Bounds() const;
	
public:
	Details::RenderSystem::NativeRenderTarget2D* NativeRenderTarget2D();

private:
	std::unique_ptr<Details::RenderSystem::NativeRenderTarget2D> nativeRenderTarget2D;
	std::int32_t pixelWidth;
	std::int32_t pixelHeight;
	std::uint32_t levelCount;
	SurfaceFormat format;
	DepthFormat depthStencilFormat;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERTARGET2D_65AA5E90_1779_4953_8BD3_0B0833D5BF36_HPP)
