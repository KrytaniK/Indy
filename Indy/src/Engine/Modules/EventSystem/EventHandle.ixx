module;

#include <typeindex>

export module EventSystem:EventHandle;

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