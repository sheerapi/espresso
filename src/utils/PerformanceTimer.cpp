#include "utils/PerformanceTimer.h"
#ifdef DEBUG
#	include "core/Application.h"
#	include "core/log.h"

namespace core::internals
{
	PerformanceTimer::PerformanceTimer(const std::string& name)
	{
		if (core::verbose)
		{
			_name = name;
			_start = std::chrono::high_resolution_clock::now();
		}
	}

	PerformanceTimer::~PerformanceTimer()
	{
		if (core::verbose)
		{
			auto end = std::chrono::high_resolution_clock::now();
			auto duration =
				std::chrono::duration_cast<std::chrono::microseconds>(end - _start)
					.count();

			if (duration < 1000)
			{
				log_debug(
					"%s took %dµs to run", _name.c_str(),
					std::chrono::duration_cast<std::chrono::microseconds>(end - _start)
						.count());
				return;
			}

			if (duration > 1000 && duration < 1000000)
			{
				log_debug(
					"%s took %dms to run", _name.c_str(),
					std::chrono::duration_cast<std::chrono::milliseconds>(end - _start)
						.count());
				return;
			}

			if (duration > 1000000)
			{
				log_warn("%s took %ds to run", _name.c_str(),
						 std::chrono::duration_cast<std::chrono::seconds>(end - _start)
							 .count());
			}
		}
	}
}
#endif