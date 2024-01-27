#pragma once

// For use by applications created using this engine

// Logging
#include "spdlog/spdlog.h"

// Event System
#include "Engine/EventSystem/Events.h" 
/* ---------- MODULES ---------- */

// Layers
#include "Engine/LayerStack/Layer.h"
#include "Engine/LayerStack/LayerStack.h"

// Renderer
#include "Engine/Renderer/Renderer.h"
/* ---------- MACROS ---------- */

/* ---------- MODULES ---------- */

import Indy_Core;

/* ---------- MACROS ---------- */

#define INDY_TRACE(...)        ::Indy::ClientLoggingContext::Get()->trace(__VA_ARGS__)
#define INDY_INFO(...)         ::Indy::ClientLoggingContext::Get()->info(__VA_ARGS__)
#define INDY_WARN(...)         ::Indy::ClientLoggingContext::Get()->warn(__VA_ARGS__)
#define INDY_ERROR(...)        ::Indy::ClientLoggingContext::Get()->error(__VA_ARGS__)
#define INDY_CRITICAL(...)     ::Indy::ClientLoggingContext::Get()->critical(__VA_ARGS__)