#include "core/Application.h"
#include "core/Component.h"
#include "core/Entity.h"
#include "core/EntryPoint.h"
#include "core/Scene.h"
#include <random>

class TestComponent : public core::Component
{
public:
    float velocity;
    volatile float position;
    float acceleration{0.1F};

    float lastTime;

    void update() override
    {
        velocity += acceleration * core::time.getDelta();
        getTransform()->position.X += velocity * core::time.getDelta();

		if (getID() == 1 && core::time.getElapsed() - lastTime >= 1.F)
		{
			log_info("%f %f", position, core::time.getDelta());
			lastTime = core::time.getElapsed();
		}
	}
};

class SandboxApp : public core::Application
{
public:
    void init() override
    {
        appName = "sandbox";
        auto* scene = new core::Scene("Test Scene");
        core::Scene::changeScene(scene);

		for (size_t i = 0; i < 50000; i++)
		{
			scene->addEntity()->addComponent<TestComponent>();
		}
	}
};

es_createApp(SandboxApp);