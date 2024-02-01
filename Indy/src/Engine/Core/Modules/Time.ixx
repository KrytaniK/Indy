module;

#include <chrono>

export module Indy_Core_Time;

export constexpr double DeltaTime = 1.0 / 30.0;

// Nanoseconds
export constexpr double NANO_TO_MICRO_RATIO = 0.001;
// Millisecons
export constexpr double NANO_TO_MILLI_RATIO = 0.000001;
// Seconds
export constexpr double NANO_TO_SEC_RATIO = 0.000000001;

export
{
	namespace Indy
	{
		class Time
		{
		public:
			static std::chrono::high_resolution_clock::time_point Now();
			static const double SecondDuration(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end);
			static const double MilliDuration(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end);
			static const double MicroDuration(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end);
			static const double NanoDuration(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end);
		};
	}
}