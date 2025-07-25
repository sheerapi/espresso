#pragma once
#include "components/graphics/Camera.h"
#include "core/Component.h"
#include "core/Entity.h"
#include <string>
#include <vector>

namespace core
{
	inline unsigned short scene_id = 0;

	class Scene
	{
	public:
		Scene(std::string name = "Scene");
		static Scene* currentScene;

		auto addEntity(std::string name = "Entity", char tag = 0) -> Entity*;

		auto addEntity(Entity* entity) -> void;
		auto getEntity(unsigned int id) -> Entity*;
		auto getEntity(const std::string& name) -> Entity*;
		auto getEntities(const char& tag) -> std::vector<Entity*>;

		auto removeEntity(Entity* entity) -> void;
		auto removeEntity(unsigned int id) -> void;
		auto removeEntity(const std::string& name) -> void;
		auto removeEntities(const char& tag) -> void;

		void registerCamera(Camera* camera);
		void unregisterCamera(Camera* camera);
		void requestCameraReorder();

		auto getCameras() -> std::vector<core::Camera*>;

		void tick();
		void render();
		void start();

		static void changeScene(Scene* scene);

		void clear()
		{
			_entities.clear();
		}

		[[nodiscard]] auto getEntities() const -> std::vector<Entity*>;

		[[nodiscard]] auto getID() const
		{
			return _id;
		}

		[[nodiscard]] auto getName() const
		{
			return _name;
		}

	private:
		std::vector<std::unique_ptr<Entity>> _entities;
		std::vector<core::Camera*> _cameras;
		unsigned short _id{0};
		std::string _name;
	};
}