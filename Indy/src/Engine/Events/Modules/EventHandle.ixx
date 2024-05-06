module;

#include <typeindex>

export module Indy_Core_Events:EventHandle;

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