// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {

class Noncopyable {
public:
    Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable & operator=(const Noncopyable&) = delete;
};

} // namespace Detail
} // namespace Pomdog
