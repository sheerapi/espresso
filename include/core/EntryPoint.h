#pragma once
#include "Application.h"
#include "core/log.h"
#include "utils/PerformanceTimer.h"

extern auto createApp(int argc, const char** argv) -> core::Application*;

auto main(int argc, const char** argv) -> int
{
	core::threadName = "main";
	
	if (argc >= 2 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--verbose") == 0))
	{
		core::verbose = true;
	}

	auto* app = createApp(argc, argv);

	{
		es_stopwatchNamed("user init");
		app->init();
	}
	
	app->setup();
	app->start();
	log_info("initialized %s!", app->getName().c_str());

	auto result = app->run();

	delete app;
	return static_cast<int>(!result);
}

#define es_createApp(appName)                                                            \
	auto createApp(int argc, const char** argv) -> core::Application*                    \
	{                                                                                    \
		return new appName();                                                            \
	}
