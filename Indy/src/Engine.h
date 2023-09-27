#pragma once

// For use by applications created using this engine

// Core Application
#include "Engine/Core/Application.h"

// Logging
#include "Engine/Core/Log.h"

// Event System
#include "Engine/EventSystem/Events.h" 

#ifdef ENGINE_PLATFORM_WINDOWS
	#include "Engine/Platform/Windows/WindowsEvents.h"
#endif

// ----- Entry Point -----
#include "Engine/Core/EntryPoint.h"