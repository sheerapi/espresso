#pragma once
#include "utils/math/Matrix4.h"
#include "utils/math/Quaternion.h"
#include "utils/math/Vector3.h"

namespace core
{
	class Entity;
	
	struct Transform
	{
	public:
		math::Vector3 position;
		math::Quaternion rotation;
		math::Vector3 scale{1, 1, 1};

		void update();
		[[nodiscard]] auto getTransform() const -> math::Matrix4
		{
			return _transform;
		}

		[[nodiscard]] auto getFinalTransform() const -> math::Matrix4;

	private:
		math::Matrix4 _transform;
		core::Entity* _entity = nullptr;

		math::Vector3 _oldPosition;
		math::Quaternion _oldRotation;
		math::Vector3 _oldScale;

		friend class core::Entity;
	};
}