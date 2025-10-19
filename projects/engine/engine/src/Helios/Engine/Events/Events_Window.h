#pragma once
#include "Helios/Engine/Events/Event.h"

namespace Helios::Engine::Events
{


	class WindowCloseEvent : public Event {
	public:
		EVENT_TYPE("WindowCloseEvent")
	};


}// namespace Helios::Engine::Events
