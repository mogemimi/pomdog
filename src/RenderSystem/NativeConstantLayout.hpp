// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVECONSTANTLAYOUT_9EA37BC8_0C0C_4453_B115_8EAC2D32FCB4_HPP
#define POMDOG_NATIVECONSTANTLAYOUT_9EA37BC8_0C0C_4453_B115_8EAC2D32FCB4_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>
#include <string>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeConstantBuffer;

class NativeConstantLayout {
public:
    NativeConstantLayout() = default;
    NativeConstantLayout(NativeConstantLayout const&) = delete;
    NativeConstantLayout & operator=(NativeConstantLayout const&) = delete;

    virtual ~NativeConstantLayout() = default;

    virtual void SetConstantBuffer(std::string const& constantName,
        std::shared_ptr<NativeConstantBuffer> const& constantBuffer) = 0;

    virtual void SetConstantBuffer(std::string const& constantName) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVECONSTANTLAYOUT_9EA37BC8_0C0C_4453_B115_8EAC2D32FCB4_HPP)
