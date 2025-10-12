#pragma once

#include "Event.h"

#include <functional>
#include <unordered_map>
#include <vector>

class EventBus
{
public:
	using Listener = std::function<void(Event&)>;

	void Subscribe(EventType type, Listener listener);
	void Dispatch(Event& event);

private:
	std::unordered_map<EventType, std::vector<Listener>> listeners;
};