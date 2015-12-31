// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

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
