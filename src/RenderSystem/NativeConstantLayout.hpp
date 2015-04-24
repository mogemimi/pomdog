// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVECONSTANTLAYOUT_9EA37BC8_HPP
#define POMDOG_NATIVECONSTANTLAYOUT_9EA37BC8_HPP

#include <string>
#include <memory>

namespace Pomdog {
namespace Detail {

class NativeBuffer;

class NativeConstantLayout {
public:
    NativeConstantLayout() = default;
    NativeConstantLayout(NativeConstantLayout const&) = delete;
    NativeConstantLayout & operator=(NativeConstantLayout const&) = delete;

    virtual ~NativeConstantLayout() = default;

    virtual void SetConstantBuffer(std::string const& constantName,
        std::shared_ptr<NativeBuffer> const& constantBuffer) = 0;

    virtual void SetConstantBuffer(std::string const& constantName) = 0;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_NATIVECONSTANTLAYOUT_9EA37BC8_HPP
