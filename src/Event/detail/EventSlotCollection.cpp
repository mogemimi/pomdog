//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "EventSlotCollection.hpp"
#include <algorithm>
#include <vector>
#include <limits>
#include <utility>
#include <mutex>
#include <type_traits>
#include <Pomdog/Config/FundamentalTypes.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include "EventSlot.hpp"

namespace Pomdog {
namespace Details {
namespace EventInternal {

class EventSlotCollection::Impl
{
private:
	typedef Event event_type;
	typedef std::function<void(event_type const&)> function_type;
	typedef EventSlot<void(Event const&)> SlotType;

public:
	Impl();

	EventConnection Connect(function_type && slot, std::weak_ptr<EventSlotCollection> && slots);

	void Disconnect(SlotType const* observer);

	void Invoke(event_type const& event);

private:
	void PushBackAddedListeners();
	void EraseRemovedListeners();

private:
	std::vector<std::shared_ptr<SlotType>> observers;
	std::vector<std::shared_ptr<SlotType>> addedObservers;
	std::shared_ptr<SlotType> dummyObserver;

	std::recursive_mutex addingProtection;
	std::recursive_mutex slotsProtection;

	std::uint32_t nestedMethodCallCount;
};
//-----------------------------------------------------------------------
EventSlotCollection::Impl::Impl()
	: nestedMethodCallCount(0)
{
	POMDOG_ASSERT(!dummyObserver);
	dummyObserver = std::make_shared<SlotType>([](event_type const&){});
}
//-----------------------------------------------------------------------
EventConnection EventSlotCollection::Impl::Connect(function_type && slot, std::weak_ptr<EventSlotCollection> && slots)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(!slots.expired());

	auto observer = std::make_shared<SlotType>(std::move(slot));
	{
		std::lock_guard<std::recursive_mutex> lock(addingProtection);

		POMDOG_ASSERT(std::end(addedObservers) == std::find(std::begin(addedObservers), std::end(addedObservers), observer));
		addedObservers.push_back(observer);
	}

	return std::move(EventConnection{std::move(observer), std::move(slots)});
}
//-----------------------------------------------------------------------
void EventSlotCollection::Impl::Disconnect(SlotType const* observer)
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

	POMDOG_ASSERT(dummyObserver);
	(*iter) = dummyObserver;
}
//-----------------------------------------------------------------------
void EventSlotCollection::Impl::PushBackAddedListeners()
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
void EventSlotCollection::Impl::EraseRemovedListeners()
{
	std::lock_guard<std::recursive_mutex> lock(slotsProtection);

	observers.erase(std::remove(std::begin(observers), std::end(observers), dummyObserver),
		std::end(observers));
}
//-----------------------------------------------------------------------
template <typename T>
class ScopedRecursiveCounter
{
public:
	explicit ScopedRecursiveCounter(T & c)
		: counter(c)
	{
		static_assert(std::is_arithmetic<T>::value, "");
		++counter;
	}

	~ScopedRecursiveCounter()
	{
		--counter;
	}

	ScopedRecursiveCounter(ScopedRecursiveCounter const&) = delete;
	ScopedRecursiveCounter& operator = (ScopedRecursiveCounter const&) = delete;

private:
	T & counter;
};
//-----------------------------------------------------------------------
void EventSlotCollection::Impl::Invoke(event_type const& event)
{
	if (nestedMethodCallCount == 0) {
		PushBackAddedListeners();
	}

	if (nestedMethodCallCount >= std::numeric_limits<std::uint16_t>::max()) {
		POMDOG_THROW_EXCEPTION(std::overflow_error, "stack overflow");
	}

	try {
		ScopedRecursiveCounter<std::uint32_t> scopedCounter(nestedMethodCallCount);

		for (auto & observer: observers) {
			auto scoped = observer;
			POMDOG_ASSERT(scoped);
			scoped->operator()(event);
		}
	}
	catch (std::exception const& e) {
		throw e;
	}

	if (nestedMethodCallCount == 0) {
		EraseRemovedListeners();
	}
}
//-----------------------------------------------------------------------
EventSlotCollection::EventSlotCollection()
	: impl(new Impl)
{}
//-----------------------------------------------------------------------
EventSlotCollection::~EventSlotCollection() = default;
//-----------------------------------------------------------------------
EventConnection EventSlotCollection::Connect(function_type const& slot)
{
	POMDOG_ASSERT(impl);
	return impl->Connect(function_type{slot}, shared_from_this());
}
//-----------------------------------------------------------------------
EventConnection EventSlotCollection::Connect(function_type && slot)
{
	POMDOG_ASSERT(impl);
	return impl->Connect(std::move(slot), shared_from_this());
}
//-----------------------------------------------------------------------
void EventSlotCollection::Disconnect(EventSlot<void(Event const&)> const* observer)
{
	POMDOG_ASSERT(impl);
	impl->Disconnect(observer);
}
//-----------------------------------------------------------------------
void EventSlotCollection::Invoke(event_type const& event)
{
	POMDOG_ASSERT(impl);
	impl->Invoke(event);
}

}// namespace EventInternal
}// namespace Details
}// namespace Pomdog
