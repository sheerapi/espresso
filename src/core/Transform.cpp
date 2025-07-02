#include "core/Transform.h"
#include "core/Entity.h"

namespace core
{
	void Transform::update()
	{
        if (_oldPosition == position &&  _oldRotation == rotation && _oldScale == scale)
        {
            return;
        }

		_transform = math::Matrix4::Scale(scale) * math::Matrix4::Rotation(rotation) *
					 math::Matrix4::Translation(position);

		if (_entity != nullptr && !_entity->isOrphan())
		{
			auto* parent = _entity->getParent();
			if (!_entity->isOrphan())
			{
				_transform = parent->transform.getTransform() * _transform;
			}
		}

        _oldPosition = position;
        _oldRotation = rotation;
        _oldScale = scale;
	}

	auto Transform::getFinalTransform() const -> math::Matrix4
	{
		math::Matrix4 transform;

		/* if (Camera::main != nullptr)
		{
			if (Camera::main->Transform != this && Camera::main->Transform != nullptr)
			{
				transform = Camera::main->Transform->GetTransform() * transform;
			}

			transform = Camera::main->GetProjectionMatrix() * transform;
		}*/

		transform = transform * _transform;
		return transform;
	}
}