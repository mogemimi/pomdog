// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/forward_declarations.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/spin_lock.h"

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

namespace pomdog::detail::signals {

class POMDOG_EXPORT ConnectionBody {
public:
    virtual ~ConnectionBody();

    virtual void disconnect() = 0;

    [[nodiscard]] virtual bool
    valid() const = 0;

    [[nodiscard]] virtual std::unique_ptr<ConnectionBody>
    deepCopy() const = 0;
};

template <typename Function>
class ConnectionBodyOverride final : public ConnectionBody {
private:
    using WeakSignal = std::weak_ptr<SignalBody<Function>>;

    WeakSignal weakSignal_;
    std::optional<std::int32_t> slotIndex_;

public:
    ConnectionBodyOverride() = default;
    ConnectionBodyOverride(const ConnectionBodyOverride&) = delete;
    ConnectionBodyOverride& operator=(const ConnectionBodyOverride&) = delete;

    ConnectionBodyOverride(WeakSignal&& weakSignalIn, std::int32_t slotIndexIn)
        : weakSignal_(std::forward<WeakSignal>(weakSignalIn))
        , slotIndex_(slotIndexIn)
    {
    }

    void disconnect() override
    {
        if (slotIndex_ == std::nullopt) {
            return;
        }

        if (auto lockedSignal = weakSignal_.lock(); lockedSignal != nullptr) {
            lockedSignal->disconnect(*slotIndex_);
            weakSignal_.reset();
        }
        slotIndex_ = std::nullopt;
    }

    [[nodiscard]] bool valid() const override
    {
        if (slotIndex_ == std::nullopt) {
            return false;
        }

        if (auto lockedSignal = weakSignal_.lock(); lockedSignal != nullptr) {
            return lockedSignal->isConnected(*slotIndex_);
        }
        return false;
    }

    [[nodiscard]] std::unique_ptr<ConnectionBody>
    deepCopy() const override
    {
        auto conn = std::make_unique<ConnectionBodyOverride>();
        conn->weakSignal_ = weakSignal_;
        conn->slotIndex_ = slotIndex_;
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

    std::vector<std::pair<std::int32_t, SlotType>> observers_;
    std::vector<std::pair<std::int32_t, SlotType>> addedObservers_;

    SpinLock addingProtection_;
    SpinLock slotsProtection_;

    std::int32_t nestedMethodCallCount_ = 0;
    std::int32_t nextSlotIndex_ = 0;

public:
    SignalBody() = default;

    SignalBody(const SignalBody&) = delete;
    SignalBody& operator=(const SignalBody&) = delete;

    SignalBody(SignalBody&&) = delete;
    SignalBody& operator=(SignalBody&&) = delete;

    template <typename Function>
    [[nodiscard]] std::unique_ptr<ConnectionBodyType>
    connect(Function&& slot);

    void disconnect(std::int32_t slotIndex);

    void emit(Arguments&&... arguments);

    [[nodiscard]] std::size_t
    getInvocationCount();

    [[nodiscard]] bool
    isConnected(std::int32_t slotIndex);

private:
    void pushBackAddedListeners();

    void eraseRemovedListeners();
};

template <typename... Arguments>
template <typename Function>
auto SignalBody<void(Arguments...)>::connect(Function&& slot)
    -> std::unique_ptr<ConnectionBodyType>
{
    POMDOG_ASSERT(slot);
    std::int32_t slotIndex = 0;
    {
        std::lock_guard<SpinLock> lock{addingProtection_};

        slotIndex = nextSlotIndex_;
        ++nextSlotIndex_;

        POMDOG_ASSERT(std::find_if(
                          std::begin(addedObservers_),
                          std::end(addedObservers_),
                          [&](const auto& pair) { return pair.first == slotIndex; }) == std::end(addedObservers_));
        addedObservers_.emplace_back(slotIndex, std::forward<Function>(slot));
    }

    std::weak_ptr<SignalBody> weakSignal = this->shared_from_this();
    POMDOG_ASSERT(!weakSignal.expired());
    return std::make_unique<ConnectionBodyType>(std::move(weakSignal), slotIndex);
}

template <typename... Arguments>
void SignalBody<void(Arguments...)>::disconnect(std::int32_t slotIndex)
{
    POMDOG_ASSERT(slotIndex <= nextSlotIndex_);
    {
        std::lock_guard<SpinLock> lock{addingProtection_};

        auto iter = std::find_if(
            std::begin(addedObservers_),
            std::end(addedObservers_),
            [&](const auto& pair) { return pair.first == slotIndex; });

        if (iter != std::end(addedObservers_)) {
            addedObservers_.erase(iter);
            return;
        }
    }

    std::lock_guard<SpinLock> lock{slotsProtection_};

    auto const iter = std::find_if(
        std::begin(observers_),
        std::end(observers_),
        [&](const auto& pair) { return pair.first == slotIndex; });

    if (iter != std::end(observers_)) {
        iter->second = nullptr;
        return;
    }
}

template <typename... Arguments>
void SignalBody<void(Arguments...)>::pushBackAddedListeners()
{
    decltype(addedObservers_) temporarySlots;
    {
        std::lock_guard<SpinLock> lock{addingProtection_};
        std::swap(temporarySlots, addedObservers_);
    }
    {
        std::lock_guard<SpinLock> lock{slotsProtection_};

        for (auto& slot : temporarySlots) {
            POMDOG_ASSERT(std::find_if(
                              std::begin(observers_),
                              std::end(observers_),
                              [&](const auto& pair) { return pair.first == slot.first; }) == std::end(observers_));
            observers_.push_back(std::move(slot));
        }
    }
}

template <typename... Arguments>
void SignalBody<void(Arguments...)>::eraseRemovedListeners()
{
    std::lock_guard<SpinLock> lock{slotsProtection_};

    observers_.erase(
        std::remove_if(
            std::begin(observers_),
            std::end(observers_),
            [](const auto& pair) { return pair.second == nullptr; }),
        std::end(observers_));
}

template <typename... Arguments>
void SignalBody<void(Arguments...)>::emit(Arguments&&... arguments)
{
    if (nestedMethodCallCount_ <= 0) {
        pushBackAddedListeners();
    }

    if (nestedMethodCallCount_ >= std::numeric_limits<std::int16_t>::max()) {
        return;
    }

    POMDOG_ASSERT(nestedMethodCallCount_ >= 0);
    ++nestedMethodCallCount_;

    try {
        for (auto& pair : observers_) {
            // NOTE: Copy the function object to a temporary object to call it
            // safely because std::function can be self-destroyed during call.
            if (auto f = pair.second; f != nullptr) {
                f(std::forward<Arguments>(arguments)...);
            }
        }
    }
    catch (const std::exception& e) {
        --nestedMethodCallCount_;
        throw e;
    }

    POMDOG_ASSERT(nestedMethodCallCount_ > 0);
    --nestedMethodCallCount_;

    if (nestedMethodCallCount_ <= 0) {
        eraseRemovedListeners();
    }
}

template <typename... Arguments>
std::size_t SignalBody<void(Arguments...)>::getInvocationCount()
{
    auto count = [this]() -> std::size_t {
        std::lock_guard<SpinLock> lock{addingProtection_};
        return addedObservers_.size();
    }();

    std::lock_guard<SpinLock> lock{slotsProtection_};
    for (auto& pair : observers_) {
        if (pair.second != nullptr) {
            ++count;
        }
    }
    return count;
}

template <typename... Arguments>
bool SignalBody<void(Arguments...)>::isConnected(std::int32_t slotIndex)
{
    {
        std::lock_guard<SpinLock> lock{slotsProtection_};

        auto iter = std::find_if(
            std::begin(observers_),
            std::end(observers_),
            [&](const auto& pair) { return pair.first == slotIndex; });

        if (iter != std::end(observers_)) {
            return (iter->second != nullptr);
        }
    }

    std::lock_guard<SpinLock> lock{addingProtection_};

    auto iter = std::find_if(
        std::begin(addedObservers_),
        std::end(addedObservers_),
        [&](const auto& pair) { return pair.first == slotIndex; });

    return (iter != std::end(addedObservers_)) && (iter->second != nullptr);
}

} // namespace pomdog::detail::signals
