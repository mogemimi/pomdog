//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MOUSECREATORDIRECTINPUT_62BF491B_21FD_4578_B48B_1AF23A69E1A3_HPP
#define POMDOG_MOUSECREATORDIRECTINPUT_62BF491B_21FD_4578_B48B_1AF23A69E1A3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../InputSystem/MouseCreator.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace InputSystem {
namespace DirectInput {

class DeviceContextDirectInput;

class MouseCreatorDirectInput final: public MouseCreator {
public:
	explicit MouseCreatorDirectInput(std::shared_ptr<DeviceContextDirectInput> const& deviceContext);

	std::shared_ptr<Mouse> Create(SubsystemScheduler & scheduler) override;

private:
	std::shared_ptr<DeviceContextDirectInput> deviceContext;
};

}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MOUSECREATORDIRECTINPUT_62BF491B_21FD_4578_B48B_1AF23A69E1A3_HPP)
