//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BUILTINSHADERPOOL_98B72B25_3798_4646_AC33_8C8B4FB9451A_HPP
#define POMDOG_BUILTINSHADERPOOL_98B72B25_3798_4646_AC33_8C8B4FB9451A_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Config/Export.hpp"
#include <typeindex>
#include <type_traits>
#include <map>
#include <memory>

namespace Pomdog {
namespace Details {

class BuiltinShaderPool {
public:
	BuiltinShaderPool() = default;
	BuiltinShaderPool(BuiltinShaderPool const&) = delete;
	BuiltinShaderPool & operator=(BuiltinShaderPool const&) = delete;
	BuiltinShaderPool(BuiltinShaderPool &&) = delete;
	BuiltinShaderPool & operator=(BuiltinShaderPool &&) = delete;

	template <class Trait>
	std::shared_ptr<EffectPass> Create(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	{
		std::type_index key{typeid(Trait)};
	
		auto iter = effects.find(key);
		if (iter != std::end(effects))
		{
			if (auto effect = iter->second.lock()) {
				return effect;
			}

			effects.erase(iter);
		}
		
		POMDOG_ASSERT(effects.find(key) == std::end(effects));
		POMDOG_ASSERT(graphicsDevice);

		auto effect = Trait::Create(graphicsDevice);
		effects.insert(std::make_pair(key, effect));
		
		static_assert(std::is_same<decltype(effect), std::shared_ptr<EffectPass>>::value, "");
		return effect;
	}
	
private:
	std::map<std::type_index, std::weak_ptr<EffectPass>> effects;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_BUILTINSHADERPOOL_98B72B25_3798_4646_AC33_8C8B4FB9451A_HPP)
