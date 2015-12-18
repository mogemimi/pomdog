// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {

class NativeRenderTarget2D {
public:
    NativeRenderTarget2D() = default;
    NativeRenderTarget2D(NativeRenderTarget2D const&) = delete;
    NativeRenderTarget2D & operator=(NativeRenderTarget2D const&) = delete;

    virtual ~NativeRenderTarget2D() = default;
};

} // namespace Detail
} // namespace Pomdog
