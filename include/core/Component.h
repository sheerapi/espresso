#pragma once
#include "core/Transform.h"
#include <memory>

namespace core
{
	inline unsigned long component_id = 0;
    class Entity;

	class Component : public std::enable_shared_from_this<Component>
	{
	public:
		Component() : _id(++component_id) {};
		virtual ~Component() = default;

		virtual void start() {};
		virtual void update() {};
		virtual void render() {};

		virtual void onEnable() {};
		virtual void onDisable() {};

		void setActive(bool active)
		{
			_active = active;
			if (active)
			{
				onEnable();
			}
			else
			{
				onDisable();
			}
		}

		[[nodiscard]] auto isActive() const -> bool
		{
			return _active;
		}

		[[nodiscard]] auto getID() const -> unsigned long
		{
			return _id;
		}

        [[nodiscard]] auto getEntity() const -> Entity*
        {
            return _entity;
        }

		[[nodiscard]] auto getTransform() const -> Transform*
		{
			return _transform;
		}

	private:
		bool _active{true};
		unsigned long _id{0};
        Entity* _entity{nullptr};
		Transform* _transform{nullptr};

		friend class Entity;
	};
}