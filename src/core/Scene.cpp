#include "core/Scene.h"
#include "core/Application.h"
#include "core/Entity.h"
#include "core/log.h"
#include <utility>

namespace core
{
	Scene* Scene::currentScene{new Scene()};

	Scene::Scene(std::string name) : _id(++scene_id), _name(std::move(name))
	{
		if (currentScene == nullptr)
		{
			changeScene(this);
		}
	};

	auto Scene::getEntity(unsigned int id) -> Entity*
	{
		for (const auto& entity : _entities)
		{
			if (entity->getID() == id)
			{
				return entity.get();
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
				return entity.get();
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
				entities.push_back(entity.get());
			}
		}
		return entities;
	}

	auto Scene::removeEntity(Entity* entity) -> void
	{
		for (auto it = _entities.begin(); it != _entities.end(); ++it)
		{
			if ((*it).get() == entity)
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
		entity->_scene = this;
		_entities.push_back(std::unique_ptr<Entity>(entity));
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
		if (currentScene != nullptr)
		{
			log_trace("changed scene to \"%s\"", scene->getName().c_str());
		}

		currentScene = scene;

		if (Application::main != nullptr && Application::main->hasInit())
		{
			currentScene->start();
		}
	}

	auto Scene::getEntities() const -> std::vector<Entity*>
	{
		std::vector<Entity*> entities;
		entities.reserve(_entities.size());

		for (const auto& entity : _entities)
		{
			entities.push_back(entity.get());
		}
		return entities;
	}

	void Scene::addEntity(Entity* entity)
	{
		_entities.push_back(std::unique_ptr<Entity>(entity));
	}

	void Scene::registerCamera(Camera* camera)
	{
		_cameras.push_back(camera);
		std::sort(_cameras.begin(), _cameras.end(), [](Camera* a, Camera* b)
				  { return a->getRenderOrder() < b->getRenderOrder(); });
		log_trace("new camera registered on scene");
	}

	void Scene::unregisterCamera(Camera* camera)
	{
		auto cam = std::remove(_cameras.begin(), _cameras.end(), camera);
		std::sort(_cameras.begin(), _cameras.end(), [](Camera* a, Camera* b)
				  { return a->getRenderOrder() < b->getRenderOrder(); });
	}

	void Scene::requestCameraReorder()
	{
		std::sort(_cameras.begin(), _cameras.end(), [](Camera* a, Camera* b)
				  { return a->getRenderOrder() < b->getRenderOrder(); });
	}
}