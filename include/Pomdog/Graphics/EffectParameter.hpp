// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPARAMETER_6D116595_A4BD_4566_8445_7BD68FF219B7_HPP
#define POMDOG_EFFECTPARAMETER_6D116595_A4BD_4566_8445_7BD68FF219B7_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "detail/EffectBinaryParameter.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <utility>
#include <type_traits>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeConstantBuffer;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT EffectParameter {
public:
	EffectParameter() = default;

	EffectParameter(EffectParameter const&) = delete;
	EffectParameter & operator=(EffectParameter const&) = delete;

	EffectParameter(GraphicsDevice & graphicsDevice, std::uint32_t byteConstants);

	EffectParameter(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::uint32_t byteConstants);

	~EffectParameter();

	template <typename T>
	T GetValue() const
	{
		static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
		T result;
		Detail::EffectBinaryParameter::Get<T>(*this, result);
		return std::move(result);
	}

	void GetValue(std::uint32_t byteWidth, void* result) const;

	template <typename T>
	void SetValue(T const& value)
	{
		static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
		Detail::EffectBinaryParameter::Set(*this, value);
	}

	template <typename T>
	void SetValue(T const* data, std::uint32_t count)
	{
		static_assert(std::is_pod<T>::value, "You can only use plain-old-data types.");
		Detail::EffectBinaryParameter::Set(*this, data, count);
	}

	void SetValue(void const* data, std::uint32_t byteWidth);

public:
	Detail::RenderSystem::NativeConstantBuffer* NativeConstantBuffer();

private:
	std::unique_ptr<Detail::RenderSystem::NativeConstantBuffer> nativeConstantBuffer;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPARAMETER_6D116595_A4BD_4566_8445_7BD68FF219B7_HPP)
