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

			virtual bool IsLockFree() = 0;

		private:
			IAtomic& operator=(IAtomic&) = delete;
		};

		template<typename T>
		class Atomic : public IAtomic
		{
		public:
			virtual ~Atomic() override = default;

			virtual bool IsLockFree() { return m_Value.is_lock_free(); };

			void Store(const T& value) { m_Value.store(value); };

			T Load() { return m_Value.load(); };

			const std::atomic<T>& Get() { return m_Value; };

			// Potential TODO: Implement remaining std::atomic functions

		private:
			std::atomic<T> m_Value;
		};
	}
}