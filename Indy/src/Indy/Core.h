#pragma once

#ifdef INDY_PLATFORM_WINDOWS
	#ifdef INDY_BUILD_DLL
		#define INDY_API __declspec(dllexport)
	#else
		#define INDY_API __declspec(dllimport)
	#endif	
#else
	#error Indy currently only supports Windows!
#endif