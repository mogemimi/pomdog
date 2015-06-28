// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RENDERCOMMAND_B320B370_HPP
#define POMDOG_RENDERCOMMAND_B320B370_HPP

#include <memory>
#include <typeindex>

namespace Pomdog {

class RenderCommand {
public:
    virtual ~RenderCommand() = default;

    virtual std::type_index TypeIndex() const = 0;

    virtual float DrawOrder() const = 0;
};

} // namespace Pomdog

#endif // POMDOG_RENDERCOMMAND_B320B370_HPP
