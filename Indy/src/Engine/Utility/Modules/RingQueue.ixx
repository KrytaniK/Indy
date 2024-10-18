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

			size_t Size();
			void Resize(const size_t& newCapacity);

			bool IsEmpty();
			bool IsFull();

			bool Enqueue(T& element);
			bool Enqueue(T&& element);
			bool Enqueue(const std::shared_ptr<T>& element);
			bool Enqueue(std::shared_ptr<T>&& element);

			bool Enqueue_Back(T& element);
			bool Enqueue_Back(T&& element);
			bool Enqueue_Back(const std::shared_ptr<T>& element);
			bool Enqueue_Back(std::shared_ptr<T>&& element);

			std::shared_ptr<T> Dequeue();
			std::shared_ptr<T> Dequeue_Back();

		private:
			std::vector<std::shared_ptr<T>> m_Array;
			size_t m_Head;
			size_t m_Tail;
			size_t m_Capacity;
			size_t m_Size;
		};

		template<typename T>
		RingQueue<T>::RingQueue(const size_t& capacity)
			: m_Capacity(capacity), m_Head(0), m_Tail(0), m_Size(0)
		{
			m_Array = std::vector<std::shared_ptr<T>>(capacity);
		}

		template<typename T>
		RingQueue<T>::~RingQueue()
		{

		}

		template<typename T>
		void RingQueue<T>::Resize(const size_t& newCapacity)
		{
			m_Array.resize(newCapacity);
			m_Capacity = newCapacity;
		}

		template<typename T>
		bool RingQueue<T>::IsEmpty()
		{
			return (m_Head == m_Tail) && (m_Array[m_Head] == nullptr);
		}

		template<typename T>
		bool RingQueue<T>::IsFull()
		{
			return (m_Head == m_Tail) && (m_Array[m_Head] != nullptr);
		}

		template<typename T>
		size_t RingQueue<T>::Size()
		{
			return m_Size;
		}

		template<typename T>
		bool RingQueue<T>::Enqueue(T& element)
		{
			if (IsFull())
				return false;

			// Enqueue at head
			m_Array[m_Head] = std::make_shared<T>(std::move(element));

			// Increment head
			m_Head = (m_Head + 1) % m_Capacity;

			m_Size++;
			return true;
		}

		template<typename T>
		bool RingQueue<T>::Enqueue(T&& element)
		{
			if (IsFull())
				return false;

			// Enqueue at head
			m_Array[m_Head] = std::make_shared<T>(std::move(element));

			// Increment head
			m_Head = (m_Head + 1) % m_Capacity;

			m_Size++;
			return true;
		}

		template<typename T>
		bool RingQueue<T>::Enqueue(const std::shared_ptr<T>& element)
		{
			if (IsFull())
				return false;

			// Enqueue at head
			m_Array[m_Head] = std::move(element);

			// Increment head
			m_Head = (m_Head + 1) % m_Capacity;

			m_Size++;
			return true;
		}

		template<typename T>
		bool RingQueue<T>::Enqueue(std::shared_ptr<T>&& element)
		{
			if (IsFull())
				return false;

			// Enqueue at head
			m_Array[m_Head] = std::move(element);

			// Increment head
			m_Head = (m_Head + 1) % m_Capacity;

			m_Size++;
			return true;
		}

		template<typename T>
		bool RingQueue<T>::Enqueue_Back(T& element)
		{
			if (IsFull())
				return false;

			// Decrement the tail by 1
			m_Tail = (m_Tail + m_Capacity - 1) % m_Capacity;

			// Enqueue at tail
			m_Array[m_Tail] = std::make_shared<T>(std::move(element));

			m_Size++;
			return true;
		}

		template<typename T>
		bool RingQueue<T>::Enqueue_Back(T&& element)
		{
			if (IsFull())
				return false;

			// Decrement the tail by 1
			m_Tail = (m_Tail + m_Capacity - 1) % m_Capacity;

			// Enqueue at tail
			m_Array[m_Tail] = std::make_shared<T>(std::move(element));

			m_Size++;
			return true;
		}

		template<typename T>
		bool RingQueue<T>::Enqueue_Back(const std::shared_ptr<T>& element)
		{
			if (IsFull())
				return false;

			// Decrement the tail by 1
			m_Tail = (m_Tail + m_Capacity - 1) % m_Capacity;

			// Enqueue at tail
			m_Array[m_Tail] = std::move(element);

			m_Size++;
			return true;
		}

		template<typename T>
		bool RingQueue<T>::Enqueue_Back(std::shared_ptr<T>&& element)
		{
			if (IsFull())
				return false;

			// Decrement the tail by 1
			m_Tail = (m_Tail + m_Capacity - 1) % m_Capacity;

			// Enqueue at tail
			m_Array[m_Tail] = std::move(element);

			m_Size++;
			return true;
		}

		template<typename T>
		std::shared_ptr<T> RingQueue<T>::Dequeue()
		{
			if (IsEmpty())
				return nullptr;

			// Decrement the head
			m_Head = (m_Head + m_Capacity - 1) % m_Capacity;

			// Dequeue from head
			std::shared_ptr<T> element = std::move(m_Array[m_Head]);

			// Reset array value for safety
			m_Array[m_Head] = nullptr;

			m_Size--;
			return std::move(element);
		}

		template<typename T>
		std::shared_ptr<T> RingQueue<T>::Dequeue_Back()
		{
			if (IsEmpty())
				return nullptr;

			// Dequeue from tail
			std::shared_ptr<T> element = std::move(m_Array[m_Head]);

			// Reset array value for safety
			m_Array[m_Head] = nullptr;

			// Increment the tail
			m_Tail = (m_Tail + 1) % m_Capacity;

			m_Size--;
			return std::move(element);
		}
	}
}