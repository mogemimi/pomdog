// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SIGNALBODY_F551D1BA_HPP
#define POMDOG_SIGNALBODY_F551D1BA_HPP

#include "ForwardDeclarations.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Signals {

class POMDOG_EXPORT ConnectionBody {
public:
    virtual ~ConnectionBody() = default;
    virtual void Disconnect() = 0;
    virtual bool Valid() const noexcept = 0;
    virtual std::unique_ptr<ConnectionBody> DeepCopy() const = 0;
};

template <typename Function>
class ConnectionBodyOverride final: public ConnectionBody {
private:
    typedef std::weak_ptr<Slot<Function>> WeakSlot;
    typedef std::weak_ptr<SignalBody<Function>> WeakSignal;

    WeakSignal weakSignal;
    WeakSlot weakSlot;

public:
    ConnectionBodyOverride(WeakSignal && weakSignalIn, WeakSlot && weakSlotIn)
        : weakSignal(std::forward<WeakSignal>(weakSignalIn))
        , weakSlot(std::forward<WeakSlot>(weakSlotIn))
    {}

    ConnectionBodyOverride(WeakSignal const& weakSignalIn, WeakSlot const& weakSlotIn)
        : weakSignal(weakSignalIn)
        , weakSlot(weakSlotIn)
    {}

    void Disconnect() override
    {
        if (weakSlot.expired()) {
            return;
        }

        auto lockedSlot = weakSlot.lock();

        if (auto lockedSignal = weakSignal.lock()) {
            lockedSignal->Disconnect(lockedSlot.get());
            weakSignal.reset();
        }

        weakSlot.reset();
    }

    bool Valid() const noexcept override
    {
        return !weakSlot.expired() && !weakSignal.expired();
    }

    std::unique_ptr<ConnectionBody> DeepCopy() const override
    {
        return std::make_unique<ConnectionBodyOverride>(weakSignal, weakSlot);
    }
};

template <typename...Arguments>
class SignalBody<void(Arguments...)> final
    : public std::enable_shared_from_this<SignalBody<void(Arguments...)>> {
private:
    typedef Slot<void(Arguments...)> SlotType;
    typedef ConnectionBodyOverride<void(Arguments...)> ConnectionBodyType;

public:
    SignalBody() = default;

    SignalBody(SignalBody const&) = delete;
    SignalBody & operator=(SignalBody const&) = delete;

    SignalBody(SignalBody &&) = delete;///@todo
    SignalBody & operator=(SignalBody &&) = delete;///@todo

    template <typename Function>
    std::unique_ptr<ConnectionBodyType> Connect(Function && slot);

    void Disconnect(SlotType const* observer);

    void operator()(Arguments &&... arguments);

    std::size_t InvocationCount() const;

private:
    void PushBackAddedListeners();
    void EraseRemovedListeners();

private:
    std::vector<std::shared_ptr<SlotType>> observers;
    std::vector<std::shared_ptr<SlotType>> addedObservers;

    std::recursive_mutex addingProtection;
    std::recursive_mutex slotsProtection;

    std::int32_t nestedMethodCallCount = 0;
};

//-----------------------------------------------------------------------
template <typename...Arguments>
template <typename Function>
auto SignalBody<void(Arguments...)>::Connect(Function && slot)
    ->std::unique_ptr<ConnectionBodyType>
{
    POMDOG_ASSERT(slot);
    auto observer = std::make_shared<SlotType>(std::forward<Function>(slot));
    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection);

        POMDOG_ASSERT(std::end(addedObservers) == std::find(
            std::begin(addedObservers), std::end(addedObservers), observer));
        addedObservers.push_back(observer);
    }

    std::weak_ptr<SignalBody> weakSignal = this->shared_from_this();
    POMDOG_ASSERT(!weakSignal.expired());
    return std::make_unique<ConnectionBodyType>(std::move(weakSignal), observer);
}
//-----------------------------------------------------------------------
template <typename...Arguments>
void SignalBody<void(Arguments...)>::Disconnect(SlotType const* observer)
{
    POMDOG_ASSERT(observer);
    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection);

        addedObservers.erase(std::remove_if(std::begin(addedObservers), std::end(addedObservers),
            [observer](std::shared_ptr<SlotType> const& p) {
                return p.get() == observer;
            }),
            std::end(addedObservers));
    }

    auto const iter = std::find_if(std::begin(observers), std::end(observers),
        [observer](std::shared_ptr<SlotType> const& p) {
            return p.get() == observer;
        });

    if (std::end(observers) == iter) {
        // FUS RO DAH
        return;
    }

    iter->reset();
}
//-----------------------------------------------------------------------
template <typename...Arguments>
void SignalBody<void(Arguments...)>::PushBackAddedListeners()
{
    std::vector<std::shared_ptr<SlotType>> temporarySlots;
    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection);
        std::swap(temporarySlots, addedObservers);
    }
    {
        std::lock_guard<std::recursive_mutex> lock(slotsProtection);

        for (auto & slot: temporarySlots) {
            POMDOG_ASSERT(std::end(observers) == std::find(
                std::begin(observers), std::end(observers), slot));
            observers.push_back(slot);
        }
    }
}
//-----------------------------------------------------------------------
template <typename...Arguments>
void SignalBody<void(Arguments...)>::EraseRemovedListeners()
{
    std::lock_guard<std::recursive_mutex> lock(slotsProtection);

    observers.erase(std::remove_if(std::begin(observers), std::end(observers),
        [](std::shared_ptr<SlotType> const& slot){ return !slot; }),
        std::end(observers));
}
//-----------------------------------------------------------------------
template <typename...Arguments>
void SignalBody<void(Arguments...)>::operator()(Arguments &&... arguments)
{
    if (nestedMethodCallCount <= 0) {
        PushBackAddedListeners();
    }

    if (nestedMethodCallCount >= std::numeric_limits<std::int16_t>::max()) {
        return;
    }

    POMDOG_ASSERT(nestedMethodCallCount >= 0);
    ++nestedMethodCallCount;

    try {
        for (auto & observer: observers) {
            if (auto scoped = observer) {
                scoped->operator()(std::forward<Arguments>(arguments)...);
            }
        }
    }
    catch (std::exception const& e) {
        --nestedMethodCallCount;
        throw e;
    }

    POMDOG_ASSERT(nestedMethodCallCount > 0);
    --nestedMethodCallCount;

    if (nestedMethodCallCount <= 0) {
        EraseRemovedListeners();
    }
}
//-----------------------------------------------------------------------
template <typename...Arguments>
std::size_t SignalBody<void(Arguments...)>::InvocationCount() const
{
    return observers.size();
}

} // namespace Signals
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_SIGNALBODY_F551D1BA_HPP
