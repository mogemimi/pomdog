// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/signals/forward_declarations.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/spin_lock.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Signals {

class POMDOG_EXPORT ConnectionBody {
public:
    virtual ~ConnectionBody();
    virtual void Disconnect() = 0;
    [[nodiscard]] virtual bool Valid() const = 0;
    [[nodiscard]] virtual std::unique_ptr<ConnectionBody> DeepCopy() const = 0;
};

template <typename Function>
class ConnectionBodyOverride final : public ConnectionBody {
private:
    using WeakSignal = std::weak_ptr<SignalBody<Function>>;

    WeakSignal weakSignal;
    std::optional<std::int32_t> slotIndex;

public:
    ConnectionBodyOverride() = default;
    ConnectionBodyOverride(const ConnectionBodyOverride&) = delete;
    ConnectionBodyOverride& operator=(const ConnectionBodyOverride&) = delete;

    ConnectionBodyOverride(WeakSignal&& weakSignalIn, std::int32_t slotIndexIn)
        : weakSignal(std::forward<WeakSignal>(weakSignalIn))
        , slotIndex(slotIndexIn)
    {
    }

    void Disconnect() override
    {
        if (slotIndex == std::nullopt) {
            return;
        }

        if (auto lockedSignal = weakSignal.lock(); lockedSignal != nullptr) {
            lockedSignal->Disconnect(*slotIndex);
            weakSignal.reset();
        }
        slotIndex = std::nullopt;
    }

    [[nodiscard]] bool Valid() const override
    {
        if (slotIndex == std::nullopt) {
            return false;
        }

        if (auto lockedSignal = weakSignal.lock(); lockedSignal != nullptr) {
            return lockedSignal->IsConnected(*slotIndex);
        }
        return false;
    }

    [[nodiscard]] std::unique_ptr<ConnectionBody> DeepCopy() const override
    {
        auto conn = std::make_unique<ConnectionBodyOverride>();
        conn->weakSignal = weakSignal;
        conn->slotIndex = slotIndex;
#if defined(__GNUC__) && !defined(__clang__)
        return conn;
#else
        return std::move(conn);
#endif
    }
};

template <typename... Arguments>
class SignalBody<void(Arguments...)> final
    : public std::enable_shared_from_this<SignalBody<void(Arguments...)>> {
private:
    using SlotType = Slot<void(Arguments...)>;
    using ConnectionBodyType = ConnectionBodyOverride<void(Arguments...)>;

public:
    SignalBody() = default;

    SignalBody(const SignalBody&) = delete;
    SignalBody& operator=(const SignalBody&) = delete;

    SignalBody(SignalBody&&) = delete;
    SignalBody& operator=(SignalBody&&) = delete;

    template <typename Function>
    [[nodiscard]] std::unique_ptr<ConnectionBodyType> Connect(Function&& slot);

    void Disconnect(std::int32_t slotIndex);

    void Emit(Arguments&&... arguments);

    [[nodiscard]] std::size_t GetInvocationCount();

    [[nodiscard]] bool IsConnected(std::int32_t slotIndex);

private:
    void PushBackAddedListeners();
    void EraseRemovedListeners();

private:
    std::vector<std::pair<std::int32_t, SlotType>> observers;
    std::vector<std::pair<std::int32_t, SlotType>> addedObservers;

    SpinLock addingProtection;
    SpinLock slotsProtection;

    std::int32_t nestedMethodCallCount = 0;
    std::int32_t nextSlotIndex = 0;
};

template <typename... Arguments>
template <typename Function>
auto SignalBody<void(Arguments...)>::Connect(Function&& slot)
    -> std::unique_ptr<ConnectionBodyType>
{
    POMDOG_ASSERT(slot);
    std::int32_t slotIndex = 0;
    {
        std::lock_guard<SpinLock> lock{addingProtection};

        slotIndex = nextSlotIndex;
        ++nextSlotIndex;

        POMDOG_ASSERT(std::end(addedObservers) == std::find_if(
            std::begin(addedObservers),
            std::end(addedObservers),
            [&](const auto& pair) { return pair.first == slotIndex; }));
        addedObservers.emplace_back(slotIndex, std::forward<Function>(slot));
    }

    std::weak_ptr<SignalBody> weakSignal = this->shared_from_this();
    POMDOG_ASSERT(!weakSignal.expired());
    return std::make_unique<ConnectionBodyType>(std::move(weakSignal), slotIndex);
}

template <typename... Arguments>
void SignalBody<void(Arguments...)>::Disconnect(std::int32_t slotIndex)
{
    POMDOG_ASSERT(slotIndex <= nextSlotIndex);
    {
        std::lock_guard<SpinLock> lock{addingProtection};

        auto iter = std::find_if(
            std::begin(addedObservers),
            std::end(addedObservers),
            [&](const auto& pair) { return pair.first == slotIndex; });

        if (iter != std::end(addedObservers)) {
            addedObservers.erase(iter);
            return;
        }
    }

    std::lock_guard<SpinLock> lock{slotsProtection};

    auto const iter = std::find_if(
        std::begin(observers),
        std::end(observers),
        [&](const auto& pair) { return pair.first == slotIndex; });

    if (iter != std::end(observers)) {
        iter->second = nullptr;
        return;
    }
}

template <typename... Arguments>
void SignalBody<void(Arguments...)>::PushBackAddedListeners()
{
    decltype(addedObservers) temporarySlots;
    {
        std::lock_guard<SpinLock> lock{addingProtection};
        std::swap(temporarySlots, addedObservers);
    }
    {
        std::lock_guard<SpinLock> lock{slotsProtection};

        for (auto& slot : temporarySlots) {
            POMDOG_ASSERT(std::end(observers) == std::find_if(
                std::begin(observers),
                std::end(observers),
                [&](const auto& pair) { return pair.first == slot.first; }));
            observers.push_back(std::move(slot));
        }
    }
}

template <typename... Arguments>
void SignalBody<void(Arguments...)>::EraseRemovedListeners()
{
    std::lock_guard<SpinLock> lock{slotsProtection};

    observers.erase(
        std::remove_if(
            std::begin(observers),
            std::end(observers),
            [](const auto& pair) { return pair.second == nullptr; }),
        std::end(observers));
}

template <typename... Arguments>
void SignalBody<void(Arguments...)>::Emit(Arguments&&... arguments)
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
        for (auto& pair : observers) {
            // NOTE: Copy the function object to a temporary object to call it
            // safely because std::function can be self-destroyed during call.
            if (auto f = pair.second; f != nullptr) {
                f(std::forward<Arguments>(arguments)...);
            }
        }
    }
    catch (const std::exception& e) {
        --nestedMethodCallCount;
        throw e;
    }

    POMDOG_ASSERT(nestedMethodCallCount > 0);
    --nestedMethodCallCount;

    if (nestedMethodCallCount <= 0) {
        EraseRemovedListeners();
    }
}

template <typename... Arguments>
std::size_t SignalBody<void(Arguments...)>::GetInvocationCount()
{
    auto count = [this]() -> std::size_t {
        std::lock_guard<SpinLock> lock{addingProtection};
        return addedObservers.size();
    }();

    std::lock_guard<SpinLock> lock{slotsProtection};
    for (auto& pair : observers) {
        if (pair.second != nullptr) {
            ++count;
        }
    }
    return count;
}

template <typename... Arguments>
bool SignalBody<void(Arguments...)>::IsConnected(std::int32_t slotIndex)
{
    {
        std::lock_guard<SpinLock> lock{slotsProtection};

        auto iter = std::find_if(
            std::begin(observers),
            std::end(observers),
            [&](const auto& pair) { return pair.first == slotIndex; });

        if (iter != std::end(observers)) {
            return (iter->second != nullptr);
        }
    }

    std::lock_guard<SpinLock> lock{addingProtection};

    auto iter = std::find_if(
        std::begin(addedObservers),
        std::end(addedObservers),
        [&](const auto& pair) { return pair.first == slotIndex; });

    return (iter != std::end(addedObservers)) && (iter->second != nullptr);
}

} // namespace Pomdog::Detail::Signals
