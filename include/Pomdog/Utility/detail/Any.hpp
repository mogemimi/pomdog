//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANY_2C2DCE41_BD36_4065_BD33_D045184AE06C_HPP
#define POMDOG_ANY_2C2DCE41_BD36_4065_BD33_D045184AE06C_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Config/Export.hpp"
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <memory>
#include <utility>

namespace Pomdog {
namespace Details {

class POMDOG_EXPORT Any final {
private:
	struct HolderBase {
		virtual ~HolderBase() = default;
	};
	
	template <typename T>
	struct Holder final: public HolderBase {
		T Value;
	
		template <typename U>
		explicit Holder(U && valueIn)
			: Value(std::forward<U>(valueIn))
		{}
		
		static_assert(std::is_object<T>::value, "");
	};

	std::unique_ptr<HolderBase> data;
	std::type_index typeIndex;

public:
	Any() = delete;
	Any(Any const&) = delete;
	Any(Any &&) = default;

	Any& operator=(Any const&) = delete;
	Any& operator=(Any &&) = default;
	
	template <typename T>
	Any(T && value)
		: data(std::make_unique<Holder<typename std::remove_reference<T>::type>>(std::forward<T>(value)))
		, typeIndex(typeid(T))
	{}
	
	template <typename T>
	bool Is() const
	{
		return typeIndex == typeid(T);
	}
	
	template <typename T>
	T const& As() const
	{
		POMDOG_ASSERT(typeIndex == typeid(T));
	
		if (!Is<T>()) {
			//throw BadAnyCast;
		}
		
		POMDOG_ASSERT(data);
		auto derived = dynamic_cast<Holder<T>*>(data.get());
		POMDOG_ASSERT(derived);
		return derived->Value;
	}
	
	template <typename T>
	T & As()
	{
		POMDOG_ASSERT(typeIndex == typeid(T));
	
		if (!Is<T>()) {
			//throw BadAnyCast;
		}
		
		POMDOG_ASSERT(data);
		auto derived = dynamic_cast<Holder<T>*>(data.get());
		POMDOG_ASSERT(derived);
		return derived->Value;
	}
	
	std::type_index Type() const
	{
		return typeIndex;
	}
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ANY_2C2DCE41_BD36_4065_BD33_D045184AE06C_HPP)
