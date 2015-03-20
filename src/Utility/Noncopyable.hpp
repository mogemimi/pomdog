//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_NONCOPYABLE_C87EECF3_1CA8_4C76_B8B0_413B0CEF41B3_HPP
#define POMDOG_NONCOPYABLE_C87EECF3_1CA8_4C76_B8B0_413B0CEF41B3_HPP

#if _MSC_VER > 1000
#pragma once
#endif

namespace Pomdog {
namespace Detail {

class Noncopyable {
public:
	Noncopyable() = default;
	Noncopyable(Noncopyable const&) = delete;
	Noncopyable & operator=(Noncopyable const&) = delete;
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NONCOPYABLE_C87EECF3_1CA8_4C76_B8B0_413B0CEF41B3_HPP)
