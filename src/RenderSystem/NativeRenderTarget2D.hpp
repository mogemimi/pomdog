// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {

class NativeRenderTarget2D {
public:
    NativeRenderTarget2D() = default;
    NativeRenderTarget2D(const NativeRenderTarget2D&) = delete;
    NativeRenderTarget2D & operator=(const NativeRenderTarget2D&) = delete;

    virtual ~NativeRenderTarget2D() = default;
};

} // namespace Detail
} // namespace Pomdog
