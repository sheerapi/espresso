#include "components/core/LuaBehavior.h"
#include "core/Application.h"
#include "core/Component.h"
#include "core/Entity.h"
#include "core/EntryPoint.h"
#include "core/Scene.h"
#include "platform/AssetManager.h"

using namespace core;

class SandboxApp : public Application
{
public:
	void init() override
	{
		appName = "sandbox";
		auto* scene = Scene::currentScene;

		scene->addEntity()->addComponent<Camera>();
		scene->addEntity("Test")->addComponent<LuaBehavior>("scripts/test.lua");
		scene->addEntity("Test2")->addComponent<LuaBehavior>("scripts/test2.lua");
	}
};

es_createApp(SandboxApp);