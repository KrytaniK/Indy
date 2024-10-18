module;

#include <cstdint>

export module Indy.Multithreading:Thread;

export
{
	namespace Indy
	{
		class IThread
		{
		public:
			virtual ~IThread() = default;

			virtual uint32_t GetUsage() = 0;

			virtual void Start() = 0;

			virtual void Sleep() = 0;
			virtual void Wake() = 0;

			virtual void Join() = 0;

			// Virtual function operator override for specific use cases
			virtual void operator()() = 0;
		};
	}
}