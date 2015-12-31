// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/ClearOptions.hpp"

namespace Pomdog {

#if __cplusplus < 201402L
ClearOptions & operator|=(ClearOptions & a, ClearOptions b) noexcept
{
    a = a | b;
    return a;
}
#endif

} // namespace Pomdog
