#ifndef LAB2_HANDLE_POOL_HEADER
#define LAB2_HANDLE_POOL_HEADER

#include <cstdint>
#include <vector>

namespace lab {
	template <typename T>
	class HandlePool;

	class HandlePoolBase {
	public:
		static constexpr uint32_t s_MaxGeneration = UINT32_MAX;
		static constexpr uint32_t s_MaxHandleCount = UINT32_MAX;

	public:
		template <typename T>
		class Handle {
		public:
			Handle() noexcept = default;

			inline bool operator==(const Handle& other) const noexcept { return m_id == other.m_id; }
			inline bool operator<(const Handle& other) const noexcept { return m_id < other.m_id; }

			template <typename U>
			inline explicit operator Handle<U>() const noexcept { return Handle<U>(GetID(), GetGeneration()); }

			inline uint32_t GetID() const noexcept { return (uint64_t)(m_id & UINT32_MAX); }
			inline uint32_t GetGeneration() const noexcept { return (uint64_t)(m_id >> 32); }
			inline uint64_t GetFullInfo() const noexcept { return m_id; }
			inline static constexpr Handle Null() noexcept { return Handle(UINT32_MAX, UINT32_MAX); }

		private:
			template <typename _T>
			friend class HandlePool;

			template <typename _T>
			friend class Handle;

			explicit constexpr Handle(uint32_t id, uint32_t gen) noexcept :
				m_id((uint64_t)id | ((uint64_t)gen << 32)) {}

		private:
			uint64_t m_id;
		}; // class Handle

		template <typename T>
		class Slot {
		public:
			Slot(const Slot&) = delete;
			Slot(Slot&& other) noexcept : m_info(other.m_info) {
				other.m_info = 0;
				new ((T*)m_value) T(std::move(other.Get()));
			}

			template <typename ... Types>
			Slot(uint32_t gen, Types&& ... args) noexcept : m_info(((uint64_t)gen << 1) | 1ull | (1ull << 32)) {
				new ((T*)m_value) T(std::forward<Types>(args)...);
			}

			~Slot() { Destroy(); }

		public:
			bool IsAlive() const noexcept { return m_info & 1ull; }
			uint64_t GetGeneration() const noexcept { return (m_info >> 1) & (UINT32_MAX >> 1); }
			uint64_t GetReferences() const noexcept { return m_info >> 32; }

			T& Get() noexcept { return *(T*)m_value; }
			const T& Get() const noexcept { return *(T*)m_value; }

			void Destroy() noexcept {
				if (IsAlive()) {
					((T*)m_value)->~T();
					m_info &= ~1ull;
					m_info &= (uint64_t)UINT32_MAX;

					uint64_t gen = GetGeneration() + 1;
					m_info |= (gen << 1);
				}
			}

			template <typename ... Types>
			void Construct(Types&& ... args) noexcept {
				Destroy();
				new ((T*)m_value) T(std::forward<Types>(args)...);

				m_info |= 1ull;
				m_info |= (1ull << 32);
			}

		private:
			uint64_t m_info = 0;
			alignas(T) uint8_t m_value[sizeof(T)];
		}; // class Slot

	protected:
		template <typename T, bool IsConst>
		class _Iterator {
		public:
			using value_type = T;
			using difference_type = ptrdiff_t;
			using const_pointer = const T*;
			using const_reference = const value_type&;
			using pointer = std::conditional_t<IsConst, const T*, T*>;
			using reference = std::conditional_t<IsConst, const value_type&, value_type&>;
			using iterator_category = std::bidirectional_iterator_tag;

		public:
			_Iterator(std::vector<Slot<T>>& pool, uint32_t index) noexcept requires(!IsConst)
				: m_pool(&pool), m_index(index) {}

			_Iterator(const std::vector<Slot<T>>& pool, uint32_t index) noexcept requires(IsConst)
				: m_pool(&pool), m_index(index) {}

			_Iterator& operator=(const _Iterator&) = default;

		public:
			reference operator*() noexcept { return (*m_pool)[m_index].Get(); }
			const_reference operator*() const noexcept { return (*m_pool)[m_index].Get(); }

			pointer operator->() noexcept { return &(*m_pool)[m_index].Get(); }
			const_pointer operator->() const noexcept { return &(*m_pool)[m_index].Get(); }

			_Iterator& operator++() noexcept {
				size_t poolSize = m_pool->size();
				while (++m_index < poolSize && !(*m_pool)[m_index].IsAlive());
				return *this;
			}

			_Iterator& operator--() noexcept {
				while (--m_index > 0 && !(*m_pool)[m_index].IsAlive());
				return *this;
			}

			_Iterator operator++(int) noexcept {
				_Iterator temp{ *this };
				this->operator++();
				return temp;
			}

			_Iterator operator--(int) noexcept {
				_Iterator temp{ *this };
				this->operator--();
				return temp;
			}

			bool operator==(const _Iterator& other) const noexcept {
				return m_index == other.m_index && m_pool == other.m_pool;
			}
			bool operator!=(const _Iterator& other) const noexcept {
				return m_index != other.m_index || m_pool != other.m_pool;
			}

		private:
			uint32_t m_index = 0;
			std::conditional_t<IsConst, const std::vector<Slot<T>>*, std::vector<Slot<T>>*> m_pool = nullptr;
		}; // class _Iterator
	}; // class HandlePoolbase

	template <typename T>
	class HandlePool : public HandlePoolBase {
	public:
		using ValueHandle = Handle<T>;
		using Iterator = _Iterator<T, false>;
		using ConstIterator = _Iterator<T, true>;

	public:
		HandlePool() noexcept = default;
		HandlePool(const HandlePool&) = delete;
		HandlePool(HandlePool&& other) noexcept :
			m_values(std::move(other.m_values)), m_freeList(std::move(other.m_freeList)) {}

		~HandlePool() = default;

	public:
		bool IsValid(ValueHandle handle) const noexcept {
			return handle.GetID() < m_values.size() &&
				m_values[handle.GetID()].IsAlive() &&
				handle.GetGeneration() == m_values[handle.GetID()].GetGeneration();
		}

		template <typename ... Types>
		ValueHandle Emplace(Types&& ... args) noexcept {
			if (!m_freeList.empty()) {
				uint32_t index = m_freeList.back();

				m_freeList.pop_back();
				m_values[index].Construct(std::forward<Types>(args)...);

				return ValueHandle(index, (uint32_t)m_values[index].GetGeneration());
			}
			uint32_t index = (uint32_t)m_values.size();
			m_values.emplace_back(0, std::forward<Types>(args)...);

			return ValueHandle(index, (uint32_t)m_values[index].GetGeneration());
		}

		bool Remove(ValueHandle handle) noexcept {
			if (!IsValid(handle)) {
				return false;
			}
			m_values[handle.GetID()].Destroy();
			m_freeList.push_back(handle.GetID());

			return true;
		}

		inline T& Get(ValueHandle handle) noexcept { return m_values[handle.GetID()].Get(); }
		inline const T& Get(ValueHandle handle) const noexcept { return m_values[handle.GetID()].Get(); }

		inline bool IsEmpty() const noexcept { return m_freeList.size() == m_values.size(); }
		inline size_t GetSize() const noexcept { return m_values.size() - m_freeList.size(); }

	public:
		Iterator      begin()        noexcept { return Iterator(m_values, 0); }
		ConstIterator begin()  const noexcept { return ConstIterator(m_values, 0); }
		ConstIterator cbegin() const noexcept { return ConstIterator(m_values, 0); }

		Iterator      end()        noexcept { return Iterator(m_values, (uint32_t)m_values.size()); }
		ConstIterator end()  const noexcept { return ConstIterator(m_values, (uint32_t)m_values.size()); }
		ConstIterator cend() const noexcept { return ConstIterator(m_values, (uint32_t)m_values.size()); }

	private:
		std::vector<Slot<T>>  m_values;
		std::vector<uint32_t> m_freeList;
	}; // class HandlePool
} // namespace lab
#endif // !LAB2_HANDLE_POOL_HEADER