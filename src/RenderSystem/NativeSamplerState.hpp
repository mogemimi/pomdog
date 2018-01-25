// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {

class NativeSamplerState {
public:
    NativeSamplerState() = default;
    NativeSamplerState(const NativeSamplerState&) = delete;
    NativeSamplerState & operator=(const NativeSamplerState&) = delete;

    virtual ~NativeSamplerState() = default;
};

} // namespace Detail
} // namespace Pomdog
