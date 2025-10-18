// Original code by Denys Kryvytskyi
// https://github.com/denyskryvytskyi/ElvenEngine/tree/master
#pragma once

#include "Helios/Engine/Events/EventHandler.h"

#include <Helios/Util/ScopeRef.h>

namespace Helios::Engine {


	using EventType = std::uint32_t;
	using EventId = std::uint64_t;


	class EventManager
	{
	public:
		EventManager() = default;
		EventManager(const EventManager&) = delete;
		const EventManager& operator=(const EventManager&) = delete;

		void Shutdown();

		void Subscribe(EventType eventType, Scope<IEventHandlerWrapper>&& handler, EventId eventId);
		void Unsubscribe(EventType eventType, const std::string& handlerName, EventId eventId);
		void TriggerEvent(const Event& event_, EventId eventId);
		void QueueEvent(Scope<Event>&& event_, EventId eventId);
		void DispatchEvents();

	private:
		std::vector<std::pair<Scope<Event>, EventId>> m_eventsQueue;
		std::unordered_map<EventType, std::vector<Scope<IEventHandlerWrapper>>> m_subscribers;
		std::unordered_map<EventType, std::unordered_map<EventId, std::vector<Scope<IEventHandlerWrapper>>>> m_subscribersByEventId;
	};


	namespace Events {


		extern EventManager g_EventManager;


		template<typename EventType>
		inline void Subscribe(const EventHandler<EventType>& callback, EventId eventId = 0, const bool unsubscribeOnSuccess = false)
		{
			Scope<IEventHandlerWrapper> handler = CreateScope<EventHandlerWrapper<EventType>>(callback, unsubscribeOnSuccess);
			g_EventManager.Subscribe(EventType::GetStaticEventType(), std::move(handler), eventId);
		}


		template<typename EventType>
		inline void Unsubscribe(const EventHandler<EventType>& callback, EventId eventId = 0)
		{
			const std::string handlerName = callback.target_type().name();
			g_EventManager.Unsubscribe(EventType::GetStaticEventType(), handlerName, eventId);
		}


		inline void TriggerEvent(const Event& triggeredEvent, EventId eventId = 0)
		{
			g_EventManager.TriggerEvent(triggeredEvent, eventId);
		}


		inline void QueueEvent(Scope<Event>&& queuedEvent, EventId eventId = 0)
		{
			g_EventManager.QueueEvent(std::forward<Scope<Event>>(queuedEvent), eventId);
		}


		inline void Shutdown()
		{
			g_EventManager.Shutdown();
		}


	}// namespace Events


}// namespace Helios::Engine
