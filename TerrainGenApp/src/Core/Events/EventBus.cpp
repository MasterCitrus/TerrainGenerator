#include "EventBus.h"

void EventBus::Subscribe(EventType type, Listener listener)
{
	listeners[type].push_back(listener);
}

void EventBus::Dispatch(Event& event)
{
	auto& list = listeners[event.GetType()];

	for (auto& listener : list)
	{
		listener(event);
		if (event.handled) break;
	}
}
