//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DEPTHSTENCILSTATE_C29A4B11_5F2E_11E3_9792_A8206655A22B_HPP
#define POMDOG_DEPTHSTENCILSTATE_C29A4B11_5F2E_11E3_9792_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"
#include "DepthStencilDescription.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeDepthStencilState;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 深度ステンシルステートを定義します。
class POMDOG_EXPORT DepthStencilState: Noncopyable
{
public:
	DepthStencilState() = delete;

	DepthStencilState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, DepthStencilDescription const& description);

	virtual ~DepthStencilState();

	///@~Japanese
	/// @brief ワインディング順序が時計回りの三角形の深度ステンシルテストの結果の使用方法を取得します。
	DepthStencilOperation const& GetClockwiseFace() const;

	///@~Japanese
	/// @brief ワインディング順序が反時計回りの三角形の深度ステンシルテストの結果の使用方法を取得します。
	DepthStencilOperation const& GetCounterClockwiseFace() const;

	///@~Japanese
	/// @brief 深度テストで使用する比較関数を取得します。
	ComparisonFunction GetDepthBufferFunction() const;

	///@~Japanese
	/// @brief ステンシルテストの比較に使用する基準値を取得します。
	std::int32_t GetReferenceStencil() const;

	///@~Japanese
	/// @brief ステンシルバッファの中で、ステンシルデータの読み取り部分を識別するために適用されるマスクです。
	std::uint32_t GetStencilMask() const;

	///@~Japanese
	/// @brief ステンシルバッファの中で、ステンシルデータの書き込み部分を識別するために適用されるマスクです。
	std::uint32_t GetStencilWriteMask() const;

	///@~Japanese
	/// @brief 深度テストが有効かどうかを取得します。デフォルト値は有効 (true) です。
	/// @return 深度テストが有効な場合は true を、無効な場合は false を返却します。
	bool IsDepthBufferEnable() const;

	///@~Japanese
	/// @brief 深度バッファへの書き込みが有効かどうかを取得します。デフォルト値は有効 (true) です。
	/// @return 深度バッファへの書き込みが有効な場合は true を、
	/// 無効な場合は false を返却します。
	bool IsDepthBufferWriteEnable() const;

	///@~Japanese
	/// @brief ステンシルテストが有効かどうかを取得します。デフォルト値は無効 (false) です。
	bool IsStencilEnable() const;

	///@~Japanese
	/// @brief 深度ステンシルバッファを有効化するステートを作成します。
	/// @remarks 既定値に設定されたステートを生成します。
	/// @details このステートはデフォルトで設定されています。
	static std::shared_ptr<DepthStencilState> CreateReadWriteDepth(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 読み取り専用の深度ステンシルバッファを有効化するステートを作成します。
	static std::shared_ptr<DepthStencilState> CreateReadOnlyDepth(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	///@~Japanese
	/// @brief 深度ステンシルバッファを使用しないステートを作成します。
	static std::shared_ptr<DepthStencilState> CreateNone(std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
public:
	Details::RenderSystem::NativeDepthStencilState* GetNativeDepthStencilState();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeDepthStencilState> nativeState;
	DepthStencilDescription description;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILSTATE_C29A4B11_5F2E_11E3_9792_A8206655A22B_HPP)
