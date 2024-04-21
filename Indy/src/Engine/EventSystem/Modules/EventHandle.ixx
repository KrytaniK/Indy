module;

#include <typeindex>

export module Indy_Core_EventSystem:EventHandle;

export
{
	namespace Indy
	{
		struct IEventHandle
		{
			std::type_index eventID;
			int32_t index = 0;
		};
	}
}