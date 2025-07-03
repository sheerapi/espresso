#include "core/Scene.h"
#include "core/TickThread.h"
#include "core/Application.h"
#include "core/log.h"

namespace core
{
    float lastTime;

	void TickThread::init()
	{
		core::threadName = "script";
        Scene::currentScene->start();
	}

	void TickThread::update()
	{
		Scene::currentScene->tick();
	}

	void TickThread::shutdown()
	{
		Scene::currentScene->clear();
        delete Scene::currentScene;
	}
}