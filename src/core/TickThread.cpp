#include "core/TickThread.h"
#include "core/Application.h"
#include "core/Scene.h"

namespace core
{
    float lastTime;

	void TickThread::init()
	{
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