#include "components/core/LuaBehavior.h"
#include "core/Application.h"
#include "core/Component.h"
#include "core/Entity.h"
#include "core/EntryPoint.h"
#include "core/Scene.h"
#include "platform/EnvironmentInfo.h"

using namespace core;

class SandboxApp : public Application
{
public:
	void init() override
	{
		auto* scene = Scene::currentScene;

		scene->addEntity()->addComponent<Camera>();
		scene->addEntity("Test")->addComponent<LuaBehavior>("scripts/test.lua");

		*(volatile int*)nullptr = 42;
	}
	
	auto getAppInfo() -> platform::ApplicationInfo override
	{
		return {
			"sandbox",
			"sandbox"
		};
	}
};

es_createApp(SandboxApp);