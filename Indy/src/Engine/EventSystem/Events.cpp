#include "Engine/Core/LogMacros.h"

#include <memory>
#include <typeindex>
#include <map>

import Indy_Core_Events;

namespace Indy
{
	std::vector<std::pair<std::type_index, uint32_t>> EventManagerCSR::s_TypeIndices;
	std::vector<std::shared_ptr<IEventListener>> EventManagerCSR::s_EventListeners;

	void IEventListener::Exec(IEvent* event)
	{
		this->Internal_Exec(event);
	}
}