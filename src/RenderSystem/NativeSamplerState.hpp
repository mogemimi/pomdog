// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog::Detail {

class NativeSamplerState {
public:
    NativeSamplerState() = default;
    NativeSamplerState(const NativeSamplerState&) = delete;
    NativeSamplerState & operator=(const NativeSamplerState&) = delete;

    virtual ~NativeSamplerState() = default;
};

} // namespace Pomdog::Detail
