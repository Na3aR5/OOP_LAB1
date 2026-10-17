#ifndef LAB2_EVENT_SYSTEM_HEADER
#define LAB2_EVENT_SYSTEM_HEADER

#include <HandlePool.h>
#include <cstdlib>

namespace lab {
	template <uint32_t TypeSet>
	struct TypeCounter {
	public:
		inline static uint32_t s_Counter = 0;
	}; // struct TypeCounter

	template <uint32_t TypeSet, typename Type>
	struct TypeID {
	public:
		inline static uint32_t s_ID = TypeCounter<TypeSet>::s_Counter++;
	}; // struct TypeID

	enum class EventResult {
		Propagate,
		Consume,
	}; // enum class EventResult

	class EventSystem {
	public:
		template <typename EventType>
		inline static uint32_t GetEventID() noexcept { return TypeID<0, EventType>::s_ID; }

		struct EventEnvelope {
			uint32_t id;
			uint32_t index;
		}; // struct EventEnvelope

		struct EventListener {
			void* context;
			void* handler;
		}; // struct EventListener

		using EventListenerHandle = HandlePoolBase::Handle<EventListener>;

		class IEventChannel {
		public:
			virtual ~IEventChannel() = default;
			virtual void Clear() noexcept = 0;
		}; // class IEventChannel

		template <typename EventType, bool = std::is_empty_v<EventType>>
		class EventChannel : public IEventChannel {
		public:
			virtual void Clear() noexcept override {}
		}; // class EventChannel<EventType, true>

		template <typename EventType>
		class EventChannel<EventType, false> : public IEventChannel {
		public:
			virtual void Clear() noexcept override { m_buffer.clear(); }

			template <typename ... Types>
			uint32_t Add(Types&& ... args) noexcept {
				m_buffer.emplace_back(std::forward<Types>(args)...);
				return (uint32_t)m_buffer.size() - 1;
			}

			inline bool IsEmpty() const noexcept { return m_current == m_buffer.size(); }
			const EventType& Get(uint32_t index) noexcept { return m_buffer[index]; }

		private:
			std::vector<EventType> m_buffer;
		}; // class EventChannel<EventType, false>

	public:
		EventSystem() = default;
		EventSystem(const EventSystem&) = delete;
		EventSystem(EventSystem&&) noexcept = delete;
		~EventSystem();

		EventSystem& operator=(const EventSystem&) = delete;
		EventSystem& operator=(EventSystem&&) noexcept = delete;

	public:
		void Dispatch() noexcept;

		template <typename EventType, typename ... Types>
		void RegisterEvent(Types&& ... args) noexcept {
			if constexpr (!std::is_empty_v<EventType>) {
				EventChannel<EventType>* channel = _GetEventChannel<EventType>();
				uint32_t index = channel->Add(std::forward<Types>(args)...);
				_EmplaceEvent(GetEventID<EventType>(), index);
			}
			else {
				_EmplaceEvent(GetEventID<EventType>(), UINT32_MAX);
			}
		}

		template <typename EventType>
		EventListenerHandle StartListen(void* context, EventResult(*handler)(void*, const EventType&)) noexcept {
			uint32_t eventID = GetEventID<EventType>();
			if (eventID >= m_eventListeners.size()) {
				m_eventListeners.resize((size_t)eventID + 1);
			}
			if (eventID >= m_eventDispatchers.size()) {
				m_eventDispatchers.resize((size_t)eventID + 1);
			}
			auto handle = m_eventListeners[eventID].Emplace(EventListener{
				.context = context,
				.handler = (void*)handler
			});
			m_eventDispatchers[eventID] = &_DispatchFn<EventType>;
			return handle;
		}

		template <typename EventType>
		void StopListen(EventListenerHandle listener) noexcept {
			uint32_t eventID = GetEventID<EventType>();
			if (eventID >= m_eventListeners.size()) {
				return;
			}
			m_eventListeners[eventID].Remove(listener);
		}

	private:
		void _EmplaceEvent(uint32_t id, uint32_t index) noexcept;

		template <typename EventType>
		EventChannel<EventType>* _GetEventChannel() noexcept {
			uint32_t eventID = GetEventID<EventType>();
			if (eventID >= m_eventChannels.size() || m_eventChannels[eventID] == nullptr) {
				m_eventChannels.resize(std::max(m_eventChannels.size(), (size_t)eventID + 1));

				IEventChannel* channel = (IEventChannel*)::operator new(sizeof(EventChannel<EventType>));
				new (channel) EventChannel<EventType>();
				m_eventChannels[eventID] = channel;
			}
			return (EventChannel<EventType>*)m_eventChannels[eventID];
		}

		template <typename EventType>
		static void _DispatchFn(EventSystem* es) noexcept {
			EventEnvelope event = es->m_eventQueue[es->m_current];
			if constexpr (std::is_empty_v<EventType>) {
				for (EventListener listener : es->m_eventListeners[event.id]) {
					auto handler = static_cast<EventResult(*)(void*, const EventType&)>(listener.handler);
					if (handler(listener.context, EventType()) == EventResult::Consume) {
						break;
					}
				}
			}
			else {
				auto channel = (EventChannel<EventType>*)es->m_eventChannels[event.id];
				for (EventListener listener : es->m_eventListeners[event.id]) {
					auto handler = static_cast<EventResult(*)(void*, const EventType&)>(listener.handler);
					if (handler(listener.context, channel->Get(event.index)) == EventResult::Consume) {
						break;
					}
				}
			}
		}

	private:
		size_t								   m_current = 0;
		std::vector<EventEnvelope>			   m_eventQueue;
		std::vector<void(*)(EventSystem*)>	   m_eventDispatchers;
		std::vector<HandlePool<EventListener>> m_eventListeners;
		std::vector<IEventChannel*>			   m_eventChannels;
	}; // class EventSystem
} // namespace lab
#endif // !LAB2_EVENT_SYSTEM_HEADER