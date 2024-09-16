module;

#include <Engine/Core/LogMacros.h>

#include <utility>
#include <vector>
#include <memory>
#include <new>

export module Indy.Utility:RingQueue_LF;

import Indy.Multithreading;

export
{
	namespace Indy
	{
		template<typename T>
		class RingQueue_LF
		{
		public:
			RingQueue_LF(const size_t& capacity);
			~RingQueue_LF();

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
			alignas(std::hardware_destructive_interference_size) Atomic<size_t> m_Head;
			alignas(std::hardware_destructive_interference_size) Atomic<size_t> m_Tail;
			size_t m_Capacity;
		};

		template<typename T>
		RingQueue_LF<T>::RingQueue_LF(const size_t& capacity)
			: m_Capacity(capacity)
		{
			m_Array = std::vector<std::unique_ptr<T>>(capacity);
			m_Head.Store(0);
			m_Tail.Store(0);
		}

		template<typename T>
		RingQueue_LF<T>::~RingQueue_LF()
		{

		}

		template<typename T>
		bool RingQueue_LF<T>::IsEmpty()
		{
			size_t headIndex = m_Head.Load();
			size_t tailIndex = m_Tail.Load();
			return (m_Array[headIndex] == nullptr) && (headIndex == tailIndex);
		}

		template<typename T>
		bool RingQueue_LF<T>::IsFull()
		{
			size_t headIndex = m_Head.Load();
			size_t tailIndex = m_Tail.Load();
			return (m_Array[headIndex] != nullptr) && (headIndex == tailIndex);
		}

		template<typename T>
		void RingQueue_LF<T>::Push(std::unique_ptr<T> element)
		{
			if (IsFull())
			{
				INDY_CORE_ERROR("Failed to push element of type {0}: Queue is full.", typeid(T).name());
				return;
			}

			size_t headIndex = m_Head.Load();
			m_Array[headIndex] = std::move(element);
			m_Head.Store((headIndex + 1) % m_Capacity);
		}

		template<typename T>
		void RingQueue_LF<T>::Push(const T& element)
		{
			if (IsFull())
			{
				INDY_CORE_ERROR("Failed to push element of type {0}: Queue is full.", typeid(T).name());
				return;
			}

			size_t headIndex = m_Head.Load();
			m_Array[headIndex] = std::make_unique<T>(element);
			m_Head.Store((headIndex + 1) % m_Capacity);
		}

		template<typename T>
		void RingQueue_LF<T>::Push(T&& element)
		{
			if (IsFull())
			{
				INDY_CORE_ERROR("Failed to push element of type {0}: Queue is full.", typeid(T).name());
				return;
			}

			size_t headIndex = m_Head.Load();
			m_Array[headIndex] = std::make_unique<T>(std::forward<T>(element));
			m_Head.Store((headIndex + 1) % m_Capacity);
		}

		template<typename T>
		template<typename... Args>
		void RingQueue_LF<T>::Emplace(Args && ...args)
		{
			if (IsFull())
			{
				INDY_CORE_ERROR("Failed to emplace element of type {0}: Queue is full.", typeid(T).name());
				return;
			}

			size_t headIndex = m_Head.Load();
			m_Array[headIndex] = std::make_unique<T>(std::forward<Args>(args)...);
			m_Head.Store((headIndex + 1) % m_Capacity);
		}

		template<typename T>
		T RingQueue_LF<T>::Front()
		{
			if (IsEmpty())
			{
				INDY_CORE_ERROR("Failed to Pop() from queue: Queue is empty.");
				return T();
			}

			size_t headIndex = m_Head.Load();
			size_t frontIndex = (headIndex == 0) ? (m_Capacity - 1) : (headIndex - 1);

			return std::move(*std::move(m_Array[frontIndex]));
		}

		template<typename T>
		T RingQueue_LF<T>::Back()
		{
			if (IsEmpty())
			{
				INDY_CORE_ERROR("Failed to Pop() from queue: Queue is empty.");
				return T();
			}

			return std::move(*std::move(m_Array[m_Tail.Load()]));
		}

		template<typename T>
		void RingQueue_LF<T>::Pop()
		{
			if (IsEmpty())
			{
				INDY_CORE_ERROR("Failed to Pop() from queue: Queue is empty.");
				return;
			}

			size_t headIndex = m_Head.Load();
			size_t popIndex = (headIndex == 0) ? (m_Capacity - 1) : (headIndex - 1);

			m_Array[popIndex] = nullptr;
			m_Head.Store(popIndex);
		}

		template<typename T>
		void RingQueue_LF<T>::PopBack()
		{
			if (IsEmpty())
			{
				INDY_CORE_ERROR("Failed to Pop() from queue: Queue is empty.");
				return;
			}

			size_t tailIndex = m_Tail.Load();
			m_Array[tailIndex] = nullptr;
			m_Tail.Store((tailIndex + 1) % m_Capacity);
		}
	}
}