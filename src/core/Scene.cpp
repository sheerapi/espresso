#include "core/Scene.h"

#include "core/Application.h"
#include "core/Entity.h"
#include "core/log.h"
#include <utility>

namespace core
{
	auto Scene::getEntity(unsigned int id) -> Entity*
	{
		for (const auto& entity : _entities)
		{
			if (entity->getID() == id)
			{
				return entity;
			}
		}
		return nullptr;
	}

	auto Scene::getEntity(const std::string& name) -> Entity*
	{
		for (const auto& entity : _entities)
		{
			if (entity->getName() == name)
			{
				return entity;
			}
		}
		return nullptr;
	}

	auto Scene::getEntities(const char& tag) -> std::vector<Entity*>
	{
		std::vector<Entity*> entities;
		for (const auto& entity : _entities)
		{
			if (entity->getTag() == tag)
			{
				entities.push_back(entity);
			}
		}
		return entities;
	}

	auto Scene::removeEntity(Entity* entity) -> void
	{
		for (auto it = _entities.begin(); it != _entities.end(); ++it)
		{
			if (*it == entity)
			{
				(*it)->setActive(false);
				_entities.erase(it);
				break;
			}
		}
	}

	auto Scene::removeEntity(unsigned int id) -> void
	{
		for (auto it = _entities.begin(); it != _entities.end(); ++it)
		{
			if ((*it)->getID() == id)
			{
				(*it)->setActive(false);
				_entities.erase(it);
				break;
			}
		}
	}

	auto Scene::removeEntity(const std::string& name) -> void
	{
		for (auto it = _entities.begin(); it != _entities.end(); ++it)
		{
			if ((*it)->getName() == name)
			{
				(*it)->setActive(false);
				_entities.erase(it);
				break;
			}
		}
	}

	auto Scene::removeEntities(const char& tag) -> void
	{
		for (auto it = _entities.begin(); it != _entities.end();)
		{
			if ((*it)->getTag() == tag)
			{
				(*it)->setActive(false);
				it = _entities.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void Scene::tick()
	{
		for (const auto& entity : _entities)
		{
			entity->tick();
		}
	}

	void Scene::render()
	{
		for (const auto& entity : _entities)
		{
			entity->render();
		}
	}

	auto Scene::addEntity(std::string name, char tag) -> Entity*
	{
		auto* entity = new Entity(std::move(name), tag);
		_entities.push_back(entity);
		return entity;
	}

	void Scene::start()
	{
		for (const auto& entity : _entities)
		{
			entity->start();
		}
	}

	void Scene::changeScene(Scene* scene)
	{
		currentScene = scene;
		if (Application::main != nullptr && Application::main->hasInit())
		{
			currentScene->start();
		}

		log_trace("changed scene to \"%s\"", scene->getName().c_str());
	}
}