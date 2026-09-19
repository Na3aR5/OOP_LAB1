#include <EventSystem.h>

lab::EventSystem::~EventSystem() {
	for (IEventChannel* channel : m_eventChannels) {
		if (channel != nullptr) {
			channel->~IEventChannel();
			::operator delete(channel);
		}
	}
}

void lab::EventSystem::Dispatch() noexcept {
	size_t eventCount = m_eventQueue.size();
	size_t dispatcherCount = m_eventDispatchers.size();

	for (; m_current < eventCount; ++m_current) {
		uint32_t eventID = m_eventQueue[m_current].id;
		if (eventID >= dispatcherCount || m_eventDispatchers[eventID] == nullptr) {
			continue;
		}
		m_eventDispatchers[eventID](this);
	}

	for (IEventChannel* channel : m_eventChannels) {
		if (channel != nullptr) {
			channel->Clear();
		}
	}
	m_current = 0;
	m_eventQueue.clear();
}

void lab::EventSystem::_EmplaceEvent(uint32_t id, uint32_t index) noexcept {
	m_eventQueue.emplace_back(EventEnvelope{
		.id = id,
		.index = index
	});
}