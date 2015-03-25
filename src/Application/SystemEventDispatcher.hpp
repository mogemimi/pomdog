// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SYSTEMEVENTDISPATCHER_7D649F10_0048_43D1_97DA_92FD44BE74E4_HPP
#define POMDOG_SYSTEMEVENTDISPATCHER_7D649F10_0048_43D1_97DA_92FD44BE74E4_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Event/EventQueue.hpp"
#include "Pomdog/Input/Keys.hpp"

namespace Pomdog {
namespace Detail {

using SystemEventDispatcher = EventQueue;

struct WindowWillCloseEvent {};
struct WindowShouldCloseEvent {};

struct ViewNeedsUpdateSurfaceEvent {};
struct ViewWillStartLiveResizeEvent {};
struct ViewDidEndLiveResizeEvent {};

//struct InputCharacterEvent {
//	std::uint16_t Character;
//};

struct InputKeyDownEvent {
	Keys Key;
};

struct InputKeyUpEvent {
	Keys Key;
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SYSTEMEVENTDISPATCHER_7D649F10_0048_43D1_97DA_92FD44BE74E4_HPP)
