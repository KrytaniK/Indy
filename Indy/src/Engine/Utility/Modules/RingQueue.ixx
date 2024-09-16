module;

#include <Engine/Core/LogMacros.h>

#include <utility>
#include <vector>
#include <memory>

export module Indy.Utility:RingQueue;

export
{
	namespace Indy
	{
		template<typename T>
		class RingQueue
		{
		public:
			RingQueue(const size_t& capacity);
			~RingQueue();

			bool IsEmpty();
			bool IsFull();

			void Push(std::unique_ptr<T> element); // Explicit Move
			void Push(const T& element); // Copy
			void Push(T&& element); // Move

			template<typename... Args>
			void Emplace(Args&&... args);

			T Front();
			T Back();

			void Pop();
			void PopBack();

		private:
			std::vector<std::unique_ptr<T>> m_Array;
			size_t m_Head;
			size_t m_Tail;
			size_t m_Capacity;
		};

		template<typename T>
		RingQueue<T>::RingQueue(const size_t& capacity)
			: m_Capacity(capacity), m_Head(0), m_Tail(0)
		{
			m_Array = std::vector<std::unique_ptr<T>>(capacity);
		}

		template<typename T>
		RingQueue<T>::~RingQueue()
		{

		}

		template<typename T>
		bool RingQueue<T>::IsEmpty()
		{
			return (m_Array[m_Head] == nullptr) && (m_Head == m_Tail);
		}

		template<typename T>
		bool RingQueue<T>::IsFull()
		{
			return (m_Array[m_Head] != nullptr) && (m_Head == m_Tail);
		}

		template<typename T>
		void RingQueue<T>::Push(std::unique_ptr<T> element)
		{
			if (IsFull())
			{
				INDY_CORE_ERROR("Failed to push element of type {0}: Queue is full.", typeid(T).name());
				return;
			}

			m_Array[m_Head] = std::move(element);

			m_Head = (m_Head + 1) % m_Capacity;
		}

		template<typename T>
		void RingQueue<T>::Push(const T& element)
		{
			if (IsFull())
			{
				INDY_CORE_ERROR("Failed to push element of type {0}: Queue is full.", typeid(T).name());
				return;
			}

			m_Array[m_Head] = std::make_unique<T>(element);
			m_Head = (m_Head + 1) % m_Capacity;
		}

		template<typename T>
		void RingQueue<T>::Push(T&& element)
		{
			if (IsFull())
			{
				INDY_CORE_ERROR("Failed to push element of type {0}: Queue is full.", typeid(T).name());
				return;
			}

			m_Array[m_Head] = std::make_unique<T>(std::forward<T>(element));

			m_Head = (m_Head + 1) % m_Capacity;
		}

		template<typename T>
		template<typename... Args>
		void RingQueue<T>::Emplace(Args && ...args)
		{
			if (IsFull())
			{
				INDY_CORE_ERROR("Failed to emplace element of type {0}: Queue is full.", typeid(T).name());
				return;
			}

			m_Array[m_Head] = std::make_unique<T>(std::forward<Args>(args)...);

			m_Head = (m_Head + 1) % m_Capacity;
		}

		template<typename T>
		T RingQueue<T>::Front()
		{
			if (IsEmpty())
			{
				INDY_CORE_ERROR("Failed to Pop() from queue: Queue is empty.");
				return T();
			}

			size_t frontIndex = (m_Head == 0) ? (m_Capacity - 1) : (m_Head - 1);
			return std::move(*std::move(m_Array[frontIndex]));
		}

		template<typename T>
		T RingQueue<T>::Back()
		{
			if (IsEmpty())
			{
				INDY_CORE_ERROR("Failed to Pop() from queue: Queue is empty.");
				return T();
			}

			return std::move(*std::move(m_Array[m_Tail]));
		}

		template<typename T>
		void RingQueue<T>::Pop()
		{
			if (IsEmpty())
			{
				INDY_CORE_ERROR("Failed to Pop() from queue: Queue is empty.");
				return;
			}

			size_t popIndex = (m_Head == 0) ? (m_Capacity - 1) : (m_Head - 1);

			m_Array[popIndex] = nullptr;
			m_Head = popIndex;
		}

		template<typename T>
		void RingQueue<T>::PopBack()
		{
			if (IsEmpty())
			{
				INDY_CORE_ERROR("Failed to Pop() from queue: Queue is empty.");
				return;
			}

			m_Array[m_Tail] = nullptr;
			m_Tail = (m_Tail + 1) % m_Capacity;
		}
	}
}