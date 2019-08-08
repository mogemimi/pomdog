// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace Pomdog::Detail::Signals {

template <typename Function>
class POMDOG_EXPORT DelegateConnectionBody final : public ConnectionBody {
private:
    using WeakSignal = std::weak_ptr<DelegateBody<Function>>;

    WeakSignal weakSignal;
    std::optional<std::int32_t> slotID;

public:
    DelegateConnectionBody() = default;
    DelegateConnectionBody(const DelegateConnectionBody&) = delete;
    DelegateConnectionBody& operator=(const DelegateConnectionBody&) = delete;

    DelegateConnectionBody(WeakSignal&& weakSignalIn, std::int32_t slotIDIn)
        : weakSignal(std::forward<WeakSignal>(weakSignalIn))
        , slotID(slotIDIn)
    {
    }

    void Disconnect() override
    {
        if (slotID == std::nullopt) {
            return;
        }

        if (auto lockedSignal = weakSignal.lock(); lockedSignal != nullptr) {
            lockedSignal->Disconnect(*slotID);
            weakSignal.reset();
        }
        slotID = std::nullopt;
    }

    [[nodiscard]] bool Valid() const override
    {
        if (slotID == std::nullopt) {
            return false;
        }
        if (auto lockedSignal = weakSignal.lock(); lockedSignal != nullptr) {
            return lockedSignal->IsConnected(*slotID);
        }
        return false;
    }

    [[nodiscard]] std::unique_ptr<ConnectionBody> DeepCopy() const override
    {
        auto conn = std::make_unique<DelegateConnectionBody>();
        conn->weakSignal = weakSignal;
        conn->slotID = slotID;
        return conn;
    }
};

template <typename... Arguments>
class POMDOG_EXPORT DelegateBody<void(Arguments...)> final
    : public std::enable_shared_from_this<DelegateBody<void(Arguments...)>> {
private:
    using ConnectionBodyType = DelegateConnectionBody<void(Arguments...)>;

public:
    DelegateBody() = default;
    DelegateBody(const DelegateBody&) = delete;
    DelegateBody(DelegateBody&&) = delete;
    DelegateBody& operator=(const DelegateBody&) = delete;
    DelegateBody& operator=(DelegateBody&&) = delete;

    template <typename Function>
    [[nodiscard]] std::unique_ptr<ConnectionBodyType> Connect(Function&& slotIn)
    {
        slot = std::forward<Function>(slotIn);
        ++slotID;
        POMDOG_ASSERT(slotID > 0);

        if (slot == nullptr) {
            return nullptr;
        }

        std::weak_ptr<DelegateBody> weakSignal = this->shared_from_this();
        POMDOG_ASSERT(!weakSignal.expired());
        return std::make_unique<ConnectionBodyType>(std::move(weakSignal), slotID);
    }

    void Disconnect()
    {
        slot = nullptr;
    }

    void Disconnect(std::int32_t slotIDIn)
    {
        if (slotIDIn != slotID) {
            return;
        }
        slot = nullptr;
    }

    void Emit(Arguments... arguments)
    {
        // NOTE: Copy the function object to a temporary object to call it
        // safely because std::function can be self-destroyed during call.
        if (auto functor = slot; functor != nullptr) {
            functor(std::forward<Arguments>(arguments)...);
        }
    }

    [[nodiscard]] bool IsConnected() const noexcept
    {
        return (slotID > 0) && (slot != nullptr);
    }

    [[nodiscard]] bool IsConnected(std::int32_t slotIDIn) const noexcept
    {
        return (slotID == slotIDIn) && (slot != nullptr);
    }

private:
    std::function<void(Arguments...)> slot;
    std::int32_t slotID = 0;
};

} // namespace Pomdog::Detail::Signals
