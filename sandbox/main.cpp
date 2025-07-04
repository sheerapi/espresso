#include "core/Application.h"
#include "core/Component.h"
#include "core/Entity.h"
#include "core/EntryPoint.h"
#include "core/Scene.h"

class SandboxApp : public core::Application
{
public:
	void init() override
	{
		appName = "sandbox";
		core::Scene::currentScene->addEntity()->addComponent<core::Camera>();
	}
};

es_createApp(SandboxApp);