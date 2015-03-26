// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTURE_3A7EAD6D_HPP
#define POMDOG_TEXTURE_3A7EAD6D_HPP

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT Texture {
public:
    Texture() = default;
    Texture(Texture const&) = delete;
    Texture(Texture &&) = default;

    Texture & operator=(Texture const&) = delete;
    Texture & operator=(Texture &&) = default;

    virtual ~Texture() = default;
};

}// namespace Pomdog

#endif // POMDOG_TEXTURE_3A7EAD6D_HPP
