//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_KEYBOARDCREATORDIRECTINPUT_9AFA1F66_4BE2_41F1_A887_169B8FAC359F_HPP
#define POMDOG_KEYBOARDCREATORDIRECTINPUT_9AFA1F66_4BE2_41F1_A887_169B8FAC359F_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "../InputSystem/KeyboardCreator.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace InputSystem {
namespace DirectInput {

class DeviceContextDirectInput;

class KeyboardCreatorDirectInput final: public KeyboardCreator {
public:
	explicit KeyboardCreatorDirectInput(std::shared_ptr<DeviceContextDirectInput> const& deviceContext);

	std::shared_ptr<Keyboard> Create(SubsystemScheduler & scheduler) override;

private:
	std::shared_ptr<DeviceContextDirectInput> deviceContext;
};

}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_KEYBOARDCREATORDIRECTINPUT_9AFA1F66_4BE2_41F1_A887_169B8FAC359F_HPP)
