#pragma once

#include "spdlog/spdlog.h"

import Indy_Core;

// Core Log Macros
#define INDY_CORE_TRACE(...)        ::Indy::CoreLoggingContext::Get()->trace(__VA_ARGS__)
#define INDY_CORE_INFO(...)         ::Indy::CoreLoggingContext::Get()->info(__VA_ARGS__)
#define INDY_CORE_WARN(...)         ::Indy::CoreLoggingContext::Get()->warn(__VA_ARGS__)
#define INDY_CORE_ERROR(...)        ::Indy::CoreLoggingContext::Get()->error(__VA_ARGS__)
#define INDY_CORE_CRITICAL(...)     ::Indy::CoreLoggingContext::Get()->critical(__VA_ARGS__)