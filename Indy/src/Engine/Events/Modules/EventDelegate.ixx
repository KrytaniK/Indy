module;

#include "Engine/Core/LogMacros.h"

#include <functional>
#include <cstdint>

export module Indy_Core_EventHandler:EventDelegate;

export
{
	namespace Indy
	{
		class EventDelegate
		{
		public:
			EventDelegate(const uint32_t& id) { this->id = id; };

			void Bind(const std::function<void()>& callback)
			{
				this->func = callback;
			};

			template<class C>
			void Bind(C* instance, void(C::* callback)())
			{
				this->func = [instance, callback]() { (instance->*callback)(); };
			}

			void Exec() const { 

				if (!this->func)
				{
					INDY_CORE_ERROR("No function bound to event delegate.");
					return;
				}

				this->func(); 
			};

		public:
			uint32_t id;

		private:
			std::function<void()> func;
		};

		template<typename EventType>
		class TEventDelegate
		{
		public:
			TEventDelegate(const uint32_t& id) { this->id = id; };

			void Bind(const std::function<void(EventType*)>& callback)
			{
				this->func = callback;
			};

			template<class C>
			void Bind(C* instance, void(C::* callback)(EventType*))
			{
				this->func = [instance, callback](EventType* event) { (instance->*callback)(event); };
			}

			void Exec(EventType* event) const { 

				if (!this->func)
				{
					INDY_CORE_ERROR("No function bound to event delegate.");
					return;
				}

				this->func(event); 
			};

		public:
			uint32_t id;

		private:
			std::function<void(EventType*)> func;
		};
	}
}