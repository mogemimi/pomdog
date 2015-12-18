// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_NATIVERENDERTARGET2D_9D701F83_HPP
#define POMDOG_NATIVERENDERTARGET2D_9D701F83_HPP

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

#endif // POMDOG_NATIVERENDERTARGET2D_9D701F83_HPP
