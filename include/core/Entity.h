#pragma once
#include "Application.h"
#include "Component.h"
#include "core/Transform.h"
#include <algorithm>
#include <atomic>
#include <list>
#include <memory>
#include <utility>
#include <vector>

namespace core
{
	inline std::atomic<unsigned int> entity_id{0};

	class Scene;

	class Entity : public std::enable_shared_from_this<Entity>
	{
	public:
		Entity(std::string name = "Entity", unsigned char entityTag = 0)
			: _entityName(std::move(name)), _entityTag(entityTag), _entityID(++entity_id)
		{
		}

		template <typename T, typename... Args> auto addComponent(Args&&... args) -> T*
		{
			static_assert(std::is_base_of_v<Component, T>,
						  "T must be derived from Component");

			auto component = new T(args...);
			component->_entity = this;
			component->_active = true;
			component->_transform = &transform;

			if (Application::main && Application::main->hasInit())
			{
				component->onEnable();
				component->start();
			}

			_components.push_back(std::unique_ptr<T>(component));
			return component;
		}

		template <typename T> auto getComponent() const -> T*
		{
			static_assert(std::is_base_of_v<Component, T>,
						  "T must be derived from Component");

			for (const auto& component : _components)
			{
				if (auto casted = dynamic_cast<T*>(component.get()))
				{
					return casted;
				}
			}
			return nullptr;
		}

		template <typename T> auto getComponents() const -> std::vector<T*>
		{
			static_assert(std::is_base_of_v<Component, T>,
						  "T must be derived from Component");

			std::vector<T*> components;
			for (const auto& component : _components)
			{
				if (auto casted = dynamic_cast<T*>(component.get()))
				{
					components.push_back(casted);
				}
			}
			return components;
		}

		template <typename T> void removeComponent()
		{
			static_assert(std::is_base_of_v<Component, T>,
						  "T must be derived from Component");

			auto it = std::find_if(
				_components.begin(), _components.end(), [](const auto& component)
				{ return dynamic_cast<T*>(component.get()) != nullptr; });

			if (it != _components.end())
			{
				(*it)->setActive(false);
				_components.erase(it);
			}
		}

		template <typename T> void removeComponents()
		{
			static_assert(std::is_base_of_v<Component, T>,
						  "T must be derived from Component");

			_components.erase(
				std::remove_if(_components.begin(), _components.end(),
							   [](const auto& component)
							   {
								   if (auto casted = dynamic_cast<T*>(component.get()))
								   {
									   casted->setActive(false);
									   return true;
								   }
								   return false;
							   }),
				_components.end());
		}

		void removeAllComponents()
		{
			for (auto& component : _components)
			{
				component->setActive(false);
			}
			_components.clear();
		}

		auto addChild(std::string name = "Entity", char entityTag = 0) -> Entity*
		{
			auto* child = new Entity(std::move(name), entityTag);
			child->_parent = this;
			child->_active = true;

			
			_children.push_back(std::shared_ptr<Entity>(child));
			return child;
		}

		auto addChild(Entity* entity) -> Entity*
		{
			entity->_parent = this;
			entity->_active = true;

			
			_children.push_back(std::shared_ptr<Entity>(entity));
			return entity;
		}

		void addChild(const std::shared_ptr<Entity>& child)
		{
			if (child->isOrphan() && !isDescendant(child.get()))
			{
				child->_parent = this;
				
				_children.push_back(child);
			}
			else
			{
				child->setParent(this);
			}
		}

		auto getChild(unsigned int id) const -> Entity*
		{
			for (const auto& child : _children)
			{
				if (child->_entityID == id)
				{
					return child.get();
				}
			}
			return nullptr;
		}

		auto getChild(const std::string& name) const -> Entity*
		{
			for (const auto& child : _children)
			{
				if (child->_entityName == name)
				{
					return child.get();
				}
			}
			return nullptr;
		}

		auto getChildren(unsigned char entityTag) const -> std::vector<Entity*>
		{
			std::vector<Entity*> children;
			for (const auto& child : _children)
			{
				if (child->_entityTag == entityTag)
				{
					children.push_back(child.get());
				}
			}
			return children;
		}

		void removeChild(Entity* child)
		{
			
			_children.remove_if(
				[child](const auto& c)
				{
					if (c.get() == child)
					{
						c->setActive(false);
						return true;
					}
					return false;
				});
		}

		void removeChild(unsigned int id)
		{
			
			_children.remove_if(
				[id](const auto& child)
				{
					if (child->_entityID == id)
					{
						child->setActive(false);
						return true;
					}
					return false;
				});
		}

		void removeChild(const std::string& name)
		{
			
			_children.remove_if(
				[&name](const auto& child)
				{
					if (child->_entityName == name)
					{
						child->setActive(false);
						return true;
					}
					return false;
				});
		}

		void removeChildren(char entityTag)
		{
			
			_children.remove_if(
				[entityTag](const auto& child)
				{
					if (child->_entityTag == entityTag)
					{
						child->setActive(false);
						return true;
					}
					return false;
				});
		}

		void setParent(Entity* parent)
		{
			if (!isOrphan())
			{
				_parent->_removeChildPriv(this);
			}

			if (!parent->isDescendant(this))
			{
				parent->addChild(this);
			}
		}

		auto isDescendant(Entity* entity) const -> bool
		{
			return std::any_of(
				_children.begin(), _children.end(), [entity](const auto& child)
				{ return child.get() == entity || child->isDescendant(entity); });
		}

		void setTag(char entityTag)
		{
			_entityTag = entityTag;
		}
		auto getTag() const -> unsigned char
		{
			return _entityTag;
		}

		void setName(const std::string& name)
		{
			_entityName = name;
		}
		auto getName() const -> const std::string&
		{
			return _entityName;
		}

		auto getID() const -> unsigned int
		{
			return _entityID;
		}

		void setActive(bool active)
		{
			_active = active;
			for (auto& component : _components)
			{
				component->setActive(active);
			}
		}
		auto isActive() const -> bool
		{
			return _active;
		}

		void setVisible(bool visible)
		{
			_visible = visible;
		}

		auto isVisible() const -> bool
		{
			return _visible;
		}

		auto isOrphan() const -> bool
		{
			return _parent == nullptr;
		}

		auto getParent() const -> Entity*
		{
			return _parent;
		}

		auto getScene() -> Scene*
		{
			return _scene;
		}

		auto getChildren() -> std::vector<Entity*>
		{
			std::vector<Entity*> result;
			for (auto& child : _children)
			{
				result.push_back(child.get());
			}

			return result;
		}

		void tick()
		{
			transform.update();
			
			if (!_active)
			{
				return;
			}

			for (const auto& component : _components)
			{
				if (!component->isActive())
				{
					continue;
				}

				component->update();
			}

			for (const auto& child : _children)
			{
				child->tick();
			}
		}

		void render()
		{
			if (!_visible)
            {
                return;
            }

			for (const auto& component : _components)
			{
				if (component->isActive())
				{
					continue;
				}

				component->render();
			}

			for (const auto& child : _children)
			{
				child->render();
			}
		}

		void start()
		{
			transform._entity = this;
			
			if (_active)
			{
				for (const auto& component : _components)
				{
					component->_entity = this;
					component->_transform = &transform;

					if (component->isActive())
					{
						component->onEnable();
						component->start();
					}
				}
			}
		}

		Transform transform;

	private:
		std::vector<std::unique_ptr<Component>> _components;
		std::list<std::shared_ptr<Entity>> _children;
		Entity* _parent{nullptr};
		std::string _entityName;
		Scene* _scene{nullptr};
		unsigned char _entityTag{0};
		unsigned int _entityID{0};
		bool _active{true};
		bool _visible{true};

		void _removeChildPriv(Entity* child)
		{
			_children.remove_if(
				[child](const auto& c)
				{
					if (c.get() == child)
					{
						c->_parent = nullptr;
						return true;
					}
					return false;
				});
		}

		friend Scene;
	};
}
