// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Reactive {

template <class T>
class Observer {
public:
    virtual ~Observer() = default;

    virtual void OnNext(T value) = 0;

    virtual void OnError(/*Exception error*/) = 0;

    virtual void OnCompleted() = 0;
};

} // namespace Reactive
} // namespace Pomdog
