// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <memory>

namespace Pomdog {
namespace Detail {
namespace InputSystem {

class NativeGamepad;

std::unique_ptr<NativeGamepad> CreateGamepad();

} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
