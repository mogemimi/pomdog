//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BOOTSTRAPPERWIN32_4778D685_22F5_4D2E_B7D5_5D8F47760747_HPP
#define POMDOG_BOOTSTRAPPERWIN32_4778D685_22F5_4D2E_B7D5_5D8F47760747_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "BootstrapSettingsWin32.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class GameHost;

namespace Details {
namespace Win32 {

class BootstrapperWin32 {
public:
	explicit BootstrapperWin32(BootstrapSettingsWin32 const& settings);

	void Run(std::function<void(std::shared_ptr<GameHost> const& gameHost)> const& run);

private:
	BootstrapSettingsWin32 settings;
};

}// namespace Win32
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_BOOTSTRAPPERWIN32_4778D685_22F5_4D2E_B7D5_5D8F47760747_HPP)
