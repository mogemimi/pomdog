// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/forward_declarations.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::signals {

template <typename Function>
class POMDOG_EXPORT DelegateConnectionBody final : public ConnectionBody {
private:
    using WeakSignal = std::weak_ptr<DelegateBody<Function>>;

    WeakSignal weakSignal_;
    std::optional<std::int32_t> slotID_;

public:
    DelegateConnectionBody() = default;
    DelegateConnectionBody(const DelegateConnectionBody&) = delete;
    DelegateConnectionBody& operator=(const DelegateConnectionBody&) = delete;

    DelegateConnectionBody(WeakSignal&& weakSignalIn, std::int32_t slotIDIn)
        : weakSignal_(std::forward<WeakSignal>(weakSignalIn))
        , slotID_(slotIDIn)
    {
    }

    void disconnect() override
    {
        if (slotID_ == std::nullopt) {
            return;
        }

        if (auto lockedSignal = weakSignal_.lock(); lockedSignal != nullptr) {
            lockedSignal->disconnect(*slotID_);
            weakSignal_.reset();
        }
        slotID_ = std::nullopt;
    }

    [[nodiscard]] bool valid() const override
    {
        if (slotID_ == std::nullopt) {
            return false;
        }
        if (auto lockedSignal = weakSignal_.lock(); lockedSignal != nullptr) {
            return lockedSignal->isConnected(*slotID_);
        }
        return false;
    }

    [[nodiscard]] std::unique_ptr<ConnectionBody>
    deepCopy() const override
    {
        auto conn = std::make_unique<DelegateConnectionBody>();
        conn->weakSignal_ = weakSignal_;
        conn->slotID_ = slotID_;
#if defined(__GNUC__) && !defined(__clang__)
        return conn;
#else
        return std::move(conn);
#endif
    }
};

template <typename... Arguments>
class POMDOG_EXPORT DelegateBody<void(Arguments...)> final
    : public std::enable_shared_from_this<DelegateBody<void(Arguments...)>> {
private:
    using ConnectionBodyType = DelegateConnectionBody<void(Arguments...)>;

    std::function<void(Arguments...)> slot_;
    std::int32_t slotID_ = 0;

public:
    DelegateBody() = default;
    DelegateBody(const DelegateBody&) = delete;
    DelegateBody(DelegateBody&&) = delete;
    DelegateBody& operator=(const DelegateBody&) = delete;
    DelegateBody& operator=(DelegateBody&&) = delete;

    template <typename Function>
    [[nodiscard]] std::unique_ptr<ConnectionBodyType>
    connect(Function&& slotIn)
    {
        slot_ = std::forward<Function>(slotIn);
        ++slotID_;
        POMDOG_ASSERT(slotID_ > 0);

        if (slot_ == nullptr) {
            return nullptr;
        }

        std::weak_ptr<DelegateBody> weakSignal = this->shared_from_this();
        POMDOG_ASSERT(!weakSignal.expired());
        return std::make_unique<ConnectionBodyType>(std::move(weakSignal), slotID_);
    }

    void disconnect()
    {
        slot_ = nullptr;
    }

    void disconnect(std::int32_t slotIDIn)
    {
        if (slotIDIn != slotID_) {
            return;
        }
        slot_ = nullptr;
    }

    void emit(Arguments... arguments)
    {
        // NOTE: Copy the function object to a temporary object to call it
        // safely because std::function can be self-destroyed during call.
        if (auto functor = slot_; functor != nullptr) {
            functor(std::forward<Arguments>(arguments)...);
        }
    }

    [[nodiscard]] bool isConnected() const noexcept
    {
        return (slotID_ > 0) && (slot_ != nullptr);
    }

    [[nodiscard]] bool isConnected(std::int32_t slotIDIn) const noexcept
    {
        return (slotID_ == slotIDIn) && (slot_ != nullptr);
    }
};

} // namespace pomdog::detail::signals
