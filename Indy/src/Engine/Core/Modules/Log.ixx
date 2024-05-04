module;

// Spdlog
#include "spdlog/spdlog.h"

// General includes
#include <memory>

export module Indy_Core_Debug;

// See https://github.com/gabime/spdlog for intended use of spdlog.

export {
	namespace Indy
	{
		#ifdef INDY_ENABLE_CORE_DEBUG
			// Log context purely for use within engine library.
			class CoreLoggingContext
			{
			private:
				static std::shared_ptr<spdlog::logger> s_Logger;
			public:
				static std::shared_ptr<spdlog::logger> Get();
			};
		#endif

		// Logging context for client applications
		class ClientLoggingContext
		{
		private:
			static std::shared_ptr<spdlog::logger> s_Logger;

		public:
			static std::shared_ptr<spdlog::logger> Get();
		};
	}
}