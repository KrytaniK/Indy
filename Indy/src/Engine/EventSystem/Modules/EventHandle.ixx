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
			size_t index = 0;
		};
	}
}