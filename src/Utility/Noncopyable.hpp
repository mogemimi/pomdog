// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NONCOPYABLE_C87EECF3_HPP
#define POMDOG_NONCOPYABLE_C87EECF3_HPP

namespace Pomdog {
namespace Detail {

class Noncopyable {
public:
    Noncopyable() = default;
    Noncopyable(Noncopyable const&) = delete;
    Noncopyable & operator=(Noncopyable const&) = delete;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_NONCOPYABLE_C87EECF3_HPP
