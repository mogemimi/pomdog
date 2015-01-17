//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_NATIVEINPUTLAYOUT_D37C97FA_26B6_45B8_81EC_6E26628CC118_HPP
#define POMDOG_NATIVEINPUTLAYOUT_D37C97FA_26B6_45B8_81EC_6E26628CC118_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeInputLayout {
public:
	NativeInputLayout() = default;
	NativeInputLayout(NativeInputLayout const&) = delete;
	NativeInputLayout & operator=(NativeInputLayout const&) = delete;

	virtual ~NativeInputLayout() = default;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEINPUTLAYOUT_D37C97FA_26B6_45B8_81EC_6E26628CC118_HPP)
