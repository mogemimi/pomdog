// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PATHHELPER_BDBB516A_HPP
#define POMDOG_PATHHELPER_BDBB516A_HPP

#include <string>

namespace Pomdog {
namespace Detail {

struct PathHelper {
    static std::string Join(std::string const& path1, std::string const& path2);
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PATHHELPER_BDBB516A_HPP
