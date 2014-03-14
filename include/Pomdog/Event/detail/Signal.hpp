//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENT_DETAIL_SIGNAL_F551D1BA_C805_462F_B5E5_87CFF3302EB9_HPP
#define POMDOG_EVENT_DETAIL_SIGNAL_F551D1BA_C805_462F_B5E5_87CFF3302EB9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <limits>
#include <memory>
#include <functional>
#include <utility>
#include <vector>
#include <mutex>
#include "../../Utility/Assert.hpp"

namespace Pomdog {
namespace Details {
namespace SignalsAndSlots {

template <typename Function>
using Slot = std::function<Function>;

template <typename Function>
class Signal;

class ConnectionBody {
public:
	virtual ~ConnectionBody() = default;
	virtual void Disconnect() = 0;
};

template <typename Function>
class ConnectionBodyOverride final: public ConnectionBody {
private:
	typedef std::weak_ptr<Slot<Function>> WeakSlot;
	typedef std::weak_ptr<Signal<Function>> WeakSignal;
	
	WeakSignal weakSignal;
	WeakSlot weakSlot;

public:
	ConnectionBodyOverride(WeakSignal && weakSignalIn, WeakSlot && weakSlotIn)
		: weakSignal(std::forward<WeakSignal>(weakSignalIn))
		, weakSlot(std::forward<WeakSlot>(weakSlotIn))
	{}

	void Disconnect() override
	{
		if (weakSlot.expired()) {
			return;
		}
		
		auto locked_slot = weakSlot.lock();
		
		if (auto locked_signal = weakSignal.lock()) {
			locked_signal->Disconnect(locked_slot.get());
			weakSignal.reset();
		}

		weakSlot.reset();
	}
};

template <typename...Arguments>
class Signal<void(Arguments...)>
	: public std::enable_shared_from_this<Signal<void(Arguments...)>> {
private:
	typedef Slot<void(Arguments...)> SlotType;
	typedef ConnectionBodyOverride<void(Arguments...)> ConnectionBodyType;

public:
	Signal();
	
	Signal(Signal const&) = delete;
	Signal & operator=(Signal const&) = delete;

	Signal(Signal &&) = delete;///@todo
	Signal & operator=(Signal &&) = delete;///@todo

	~Signal() = default;

	template <typename Function>
	std::weak_ptr<ConnectionBodyType> Connect(Function && slot);
	
	void Disconnect(SlotType const* observer);

	void operator()(Arguments&&... arguments);
	
private:
	void PushBackAddedListeners();
	void EraseRemovedListeners();
	
private:
	std::vector<std::shared_ptr<SlotType>> observers;
	std::vector<std::shared_ptr<SlotType>> addedObservers;

	std::recursive_mutex addingProtection;
	std::recursive_mutex slotsProtection;

	std::uint32_t nestedMethodCallCount;
};


//-----------------------------------------------------------------------
template <typename...Arguments>
Signal<void(Arguments...)>::Signal()
	: nestedMethodCallCount(0)
{}
//-----------------------------------------------------------------------
template <typename...Arguments>
template <typename Function>
auto Signal<void(Arguments...)>::Connect(Function && slot)->std::weak_ptr<ConnectionBodyType>
{
	std::weak_ptr<Signal> weak_signal = this->shared_from_this();

	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(!weak_signal.expired());

	auto observer = std::make_shared<SlotType>(std::forward<Function>(slot));
	{
		std::lock_guard<std::recursive_mutex> lock(addingProtection);

		POMDOG_ASSERT(std::end(addedObservers) == std::find(std::begin(addedObservers), std::end(addedObservers), observer));
		addedObservers.push_back(observer);
	}

	std::shared_ptr<ConnectionBodyType> connectionBody(observer,
		new ConnectionBodyType(std::move(weak_signal), observer));

	POMDOG_ASSERT(connectionBody);
	return connectionBody;
}
//-----------------------------------------------------------------------
template <typename...Arguments>
void Signal<void(Arguments...)>::Disconnect(SlotType const* observer)
{
	POMDOG_ASSERT(observer);
	{
		std::lock_guard<std::recursive_mutex> lock(addingProtection);

		addedObservers.erase(std::remove_if(std::begin(addedObservers), std::end(addedObservers),
			[observer](std::shared_ptr<SlotType> const& p){
				return p.get() == observer;
			}),
			std::end(addedObservers));
	}

	auto const iter	= std::find_if(std::begin(observers), std::end(observers),
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
void Signal<void(Arguments...)>::PushBackAddedListeners()
{
	std::vector<std::shared_ptr<SlotType>> temporarySlots;
	{
		std::lock_guard<std::recursive_mutex> lock(addingProtection);

		std::swap(temporarySlots, addedObservers);
	}
	{
		std::lock_guard<std::recursive_mutex> lock(slotsProtection);

		for (auto & slot: temporarySlots) {
			POMDOG_ASSERT(std::end(observers) == std::find(std::begin(observers), std::end(observers), slot));
			observers.push_back(slot);
		}
	}
}
//-----------------------------------------------------------------------
template <typename...Arguments>
void Signal<void(Arguments...)>::EraseRemovedListeners()
{
	std::lock_guard<std::recursive_mutex> lock(slotsProtection);

	observers.erase(std::remove_if(std::begin(observers), std::end(observers),
		[](std::shared_ptr<SlotType> const& slot){ return !slot; }),
		std::end(observers));
}
//-----------------------------------------------------------------------
template <typename...Arguments>
void Signal<void(Arguments...)>::operator()(Arguments&&... arguments)
{
	if (nestedMethodCallCount <= 0) {
		PushBackAddedListeners();
	}

	if (nestedMethodCallCount >= std::numeric_limits<std::uint16_t>::max()) {
		return;
	}

	try {
		++nestedMethodCallCount;

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

}// namespace SignalsAndSlots
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_EVENT_DETAIL_SIGNAL_F551D1BA_C805_462F_B5E5_87CFF3302EB9_HPP)
