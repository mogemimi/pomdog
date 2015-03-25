// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RENDERCOMMAND_B320B370_4577_4D5D_A9D2_A12F0291F0E1_HPP
#define POMDOG_RENDERCOMMAND_B320B370_4577_4D5D_A9D2_A12F0291F0E1_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <memory>
#include <typeindex>

namespace Pomdog {

class GraphicsContext;

class RenderCommand {
public:
    virtual ~RenderCommand() = default;

    virtual std::type_index TypeIndex() const = 0;

    virtual float DrawOrder() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERCOMMAND_B320B370_4577_4D5D_A9D2_A12F0291F0E1_HPP)
