module;

#include <atomic>

export module Indy.Multithreading:Atomic;

export
{
	namespace Indy
	{
		class IAtomic
		{
		public:
			virtual ~IAtomic() = default;
		};

		template<typename T>
		class Atomic : public IAtomic
		{
		public:
			Atomic() {};
			Atomic(T initialValue) : m_Value(std::atomic<T>(initialValue)) {};
			virtual ~Atomic() override = default;

			bool IsLockFree() { return m_Value.is_lock_free(); };

			void Store(const T& value) { m_Value.store(value); };

			T Load(std::memory_order order = std::memory_order_seq_cst) 
			{ 
				return m_Value.load(order); 
			};

			std::atomic<T>& Get() { return m_Value; };

			T Exchange(const T& desired, std::memory_order order = std::memory_order_seq_cst)
			{
				return m_Value.exchange(desired, order);
			}

			bool CompareExchangeWeak(
				const T& expected,
				const T& desired,
				std::memory_order success,
				std::memory_order failure
			) {
				return m_Value.compare_exchange_weak(expected, desired, success, failure);
			}

			bool CompareExchangeWeak(
				const T& expected,
				const T& desired,
				std::memory_order order = std::memory_order_seq_cst
			) {
				return m_Value.compare_exchange_weak(expected, desired, order);
			}

			bool CompareExchangeStong(
				const T& expected,
				const T& desired,
				std::memory_order success,
				std::memory_order failure
			) {
				return m_Value.compare_exchange_strong(expected, desired, success, failure);
			}

			bool CompareExchangeStrong(
				const T& expected,
				const T& desired,
				std::memory_order order = std::memory_order_seq_cst
			) {
				return m_Value.compare_exchange_strong(expected, desired, order);
			}

			void Wait(const T& old, std::memory_order order = std::memory_order_seq_cst)
			{
				m_Value.wait(old, order);
			}

			void NotifyOne()
			{
				m_Value.notify_one();
			}

			void NotifyAll()
			{
				m_Value.notify_all();
			}

		private:
			Atomic<T>& operator=(const Atomic<T>&) = delete; // No explicit assignments

		private:
			std::atomic<T> m_Value;
		};
	}
}