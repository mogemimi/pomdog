//
//  Copyright (C) 2013 mogemimi.
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

public:
	Impl();

	EventConnection Connect(function_type slot, std::shared_ptr<EventSlotCollection> && slots);

	void Disconnect(EventSlot const* observer);

	void Trigger(event_type const& event);

private:
	void PushBackAddedListeners();
	void EraseRemovedListeners();

private:
	std::vector<std::shared_ptr<EventSlot> > observers;
	std::vector<std::shared_ptr<EventSlot> > addedObservers;
	std::shared_ptr<EventSlot> dummyObserver;

	std::recursive_mutex addingProtection;
	std::recursive_mutex slotsProtection;

	std::uint32_t nestedMethodCallCount;
};
//-----------------------------------------------------------------------
EventSlotCollection::Impl::Impl()
	: nestedMethodCallCount(0)
{
	POMDOG_ASSERT(!dummyObserver);
	dummyObserver = std::make_shared<EventSlot>([](event_type const&){}, std::weak_ptr<EventSlotCollection>());
}
//-----------------------------------------------------------------------
EventConnection EventSlotCollection::Impl::Connect(function_type slot, std::shared_ptr<EventSlotCollection> && slots)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(slots);

	auto observer = std::make_shared<EventSlot>(std::move(slot), std::move(slots));
	{
		std::lock_guard<std::recursive_mutex> lock(addingProtection);

		POMDOG_ASSERT(std::end(addedObservers) == std::find(std::begin(addedObservers), std::end(addedObservers), observer));
		addedObservers.push_back(observer);
	}

	return std::move(EventConnection(std::move(observer)));
}
//-----------------------------------------------------------------------
void EventSlotCollection::Impl::Disconnect(EventSlot const* observer)
{
	POMDOG_ASSERT(observer);
	{
		std::lock_guard<std::recursive_mutex> lock(addingProtection);

		addedObservers.erase(
			std::remove_if(std::begin(addedObservers), std::end(addedObservers), [observer](std::shared_ptr<EventSlot> const& p){
				return p.get() == observer;
			}),
			std::end(addedObservers)
		);
	}

	auto const iter	= std::find_if(std::begin(observers), std::end(observers), [observer](std::shared_ptr<EventSlot> const& p){
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
	std::vector<std::shared_ptr<EventSlot> > temporarySlots;
	{
		std::lock_guard<std::recursive_mutex> lock(addingProtection);

		std::swap(temporarySlots, addedObservers);
	}
	{
		std::lock_guard<std::recursive_mutex> lock(slotsProtection);

		std::for_each(std::begin(temporarySlots), std::end(temporarySlots), [this](std::shared_ptr<EventSlot> const& slot) {
			POMDOG_ASSERT(std::end(observers) == std::find(std::begin(observers), std::end(observers), slot));
			observers.push_back(slot);
		});
	}
}
//-----------------------------------------------------------------------
void EventSlotCollection::Impl::EraseRemovedListeners()
{
	std::lock_guard<std::recursive_mutex> lock(slotsProtection);

	observers.erase(
		std::remove(std::begin(observers), std::end(observers), dummyObserver),
		std::end(observers)
	);
}
//-----------------------------------------------------------------------
template <typename T>
struct ScopedRecursiveCounter
{
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

private:
	ScopedRecursiveCounter(ScopedRecursiveCounter const&);
	ScopedRecursiveCounter& operator = (ScopedRecursiveCounter const&);

	T & counter;
};
//-----------------------------------------------------------------------
void EventSlotCollection::Impl::Trigger(event_type const& event)
{
	if (nestedMethodCallCount == 0) {
		PushBackAddedListeners();
	}

	if (nestedMethodCallCount >= std::numeric_limits<std::uint16_t>::max()) {
		POMDOG_THROW_EXCEPTION(std::overflow_error,
			"stack overflow", "EventSlotCollection::Impl::Tick");
	}

	try {
		ScopedRecursiveCounter<std::uint32_t> scopedCounter(nestedMethodCallCount);

		std::for_each(std::begin(observers), std::end(observers), [&event](std::shared_ptr<EventSlot> const& observer) {
			auto const scoped = observer;
			scoped->Invoke(event);
		});
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
EventSlotCollection::~EventSlotCollection()
{}
//-----------------------------------------------------------------------
EventConnection EventSlotCollection::Connect(function_type const& slot)
{
	POMDOG_ASSERT(impl);
	return impl->Connect(slot, shared_from_this());
}
//-----------------------------------------------------------------------
EventConnection EventSlotCollection::Connect(function_type && slot)
{
	POMDOG_ASSERT(impl);
	return impl->Connect(std::move(slot), shared_from_this());
}
//-----------------------------------------------------------------------
void EventSlotCollection::Disconnect(EventSlot const* observer)
{
	POMDOG_ASSERT(impl);
	impl->Disconnect(observer);
}
//-----------------------------------------------------------------------
void EventSlotCollection::Trigger(event_type const& event)
{
	POMDOG_ASSERT(impl);
	impl->Trigger(event);
}

}// namespace EventInternal
}// namespace Details
}// namespace Pomdog
