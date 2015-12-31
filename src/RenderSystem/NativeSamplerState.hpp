// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {

class NativeSamplerState {
public:
    NativeSamplerState() = default;
    NativeSamplerState(NativeSamplerState const&) = delete;
    NativeSamplerState & operator=(NativeSamplerState const&) = delete;

    virtual ~NativeSamplerState() = default;
};

} // namespace Detail
} // namespace Pomdog
