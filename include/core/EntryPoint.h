#pragma once
#include "Application.h"
#include "core/log.h"

extern auto createApp(int argc, const char** argv) -> core::Application*;

auto main(int argc, const char** argv) -> int
{
	auto* app = createApp(argc, argv);

	app->init();
	log_info("initialized %s!", app->getName().c_str());

	app->setup();

	auto result = app->run();

	delete app;
	return static_cast<int>(!result);
}

#define es_createApp(appName)                                                            \
	auto createApp(int argc, const char** argv) -> core::Application*                    \
	{                                                                                    \
		return new appName();                                                            \
	}
