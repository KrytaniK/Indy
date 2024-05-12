#pragma once

#include "spdlog/spdlog.h"

import Indy.Log;

#ifdef ENGINE_DEBUG
	#ifdef INDY_ENABLE_CORE_DEBUG
		// Core Log Macros
		#define INDY_CORE_TRACE(...)        Indy::CoreLoggingContext::Get()->trace(__VA_ARGS__)
		#define INDY_CORE_INFO(...)         Indy::CoreLoggingContext::Get()->info(__VA_ARGS__)
		#define INDY_CORE_WARN(...)         Indy::CoreLoggingContext::Get()->warn(__VA_ARGS__)
		#define INDY_CORE_ERROR(...)        Indy::CoreLoggingContext::Get()->error(__VA_ARGS__)
		#define INDY_CORE_CRITICAL(...)     Indy::CoreLoggingContext::Get()->critical(__VA_ARGS__)
	#endif

	// Client Log Macros
	#define INDY_TRACE(...)        Indy::ClientLoggingContext::Get()->trace(__VA_ARGS__)
	#define INDY_INFO(...)         Indy::ClientLoggingContext::Get()->info(__VA_ARGS__)
	#define INDY_WARN(...)         Indy::ClientLoggingContext::Get()->warn(__VA_ARGS__)
	#define INDY_ERROR(...)        Indy::ClientLoggingContext::Get()->error(__VA_ARGS__)
	#define INDY_CRITICAL(...)     Indy::ClientLoggingContext::Get()->critical(__VA_ARGS__)
#endif