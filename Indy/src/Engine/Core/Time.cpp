#include <chrono>

import Time;

namespace Indy
{
	std::chrono::high_resolution_clock::time_point Time::Now()
	{
		return std::chrono::high_resolution_clock::now();
	}

	const double Time::SecondDuration(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end)
	{
		return (double)NanoDuration(start, end) * NANO_TO_SEC_RATIO;
	}

	const double Time::MilliDuration(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end)
	{
		return (double)NanoDuration(start, end) * NANO_TO_MILLI_RATIO;
	}

	const double Time::MicroDuration(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end)
	{
		return (double)NanoDuration(start, end) * NANO_TO_MICRO_RATIO;
	}

	const double Time::NanoDuration(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end)
	{
		return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
	}
}