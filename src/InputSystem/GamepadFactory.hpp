// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <memory>

namespace Pomdog::Detail::InputSystem {

class NativeGamepad;

std::unique_ptr<NativeGamepad> CreateGamepad();

} // namespace Pomdog::Detail::InputSystem
