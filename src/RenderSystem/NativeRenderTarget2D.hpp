// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog::Detail {

class NativeRenderTarget2D {
public:
    NativeRenderTarget2D() = default;
    NativeRenderTarget2D(const NativeRenderTarget2D&) = delete;
    NativeRenderTarget2D & operator=(const NativeRenderTarget2D&) = delete;

    virtual ~NativeRenderTarget2D() = default;
};

} // namespace Pomdog::Detail
