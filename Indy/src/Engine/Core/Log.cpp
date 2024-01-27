#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

import Indy_Core;

namespace Indy
{

#ifdef INDY_ENABLE_CORE_DEBUG
	// --------------------------------
	// ----- Core Logging Context -----

	std::shared_ptr<spdlog::logger> CoreLoggingContext::s_Logger;

	std::shared_ptr<spdlog::logger> CoreLoggingContext::Get()
	{
		if (CoreLoggingContext::s_Logger == nullptr)
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			CoreLoggingContext::s_Logger = spdlog::stdout_color_mt("[INDY-CORE]");
			CoreLoggingContext::s_Logger->set_level(spdlog::level::trace);
		}

		return s_Logger;
	}
#endif

	// ----------------------------------
	// ----- Client Logging Context -----

	std::shared_ptr<spdlog::logger> ClientLoggingContext::s_Logger;

	std::shared_ptr<spdlog::logger> ClientLoggingContext::Get()
	{
		if (ClientLoggingContext::s_Logger == nullptr)
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			ClientLoggingContext::s_Logger = spdlog::stdout_color_mt("[App]");
			ClientLoggingContext::s_Logger->set_level(spdlog::level::trace);
		}

		return s_Logger;
	}
}