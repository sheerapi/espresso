#pragma once
#ifdef DEBUG
#	include <chrono>
#	include <string>

namespace core::internals
{
	class PerformanceTimer
	{
	public:
		PerformanceTimer(const std::string& name);
		~PerformanceTimer();

	private:
		std::string _name;
		std::chrono::high_resolution_clock::time_point _start;
	};
}

#	define es_stopwatch()                                                               \
		auto stopwatch_func = core::internals::PerformanceTimer(__FUNCTION__)
#	define es_stopwatchPretty()                                                               \
		auto stopwatch_func = core::internals::PerformanceTimer(__PRETTY_FUNCTION__)
#	define es_stopwatchNamed(name)                                                               \
		auto stopwatch_func = core::internals::PerformanceTimer(name)
#else
#	define es_stopwatch()
#	define es_stopwatchPretty()
#	define es_stopwatchNamed(name)
#endif