#include "core/Transform.h"
#include "core/Entity.h"
#include "utils/math/Matrix4.h"
#include "utils/math/Quaternion.h"

namespace core
{
	void Transform::update()
	{
		_recomputeWorldMatrix();
	}
	
	void Transform::setLocalPosition(const math::Vector3& position)
	{
		_localPos = position;
		_markDirty();
	}

	void Transform::setLocalRotation(const math::Quaternion& rotation)
	{
		_localRot = rotation;
		_markDirty();
	}

	void Transform::setLocalScale(const math::Vector3& scale)
	{
		_localScale = scale;
		_markDirty();
	}

	// World property getters (trigger recomputation if dirty)
	auto Transform::getPosition() const -> math::Vector3
	{
		return _cachedWorldPos;
	}

	auto Transform::getRotation() const -> math::Quaternion
	{
		return _cachedWorldRot;
	}

	auto Transform::getLossyScale() const -> math::Vector3
	{
		math::Vector3 lossyScale;
		lossyScale.x = math::Vector3(_worldMatrix.data[0][0], _worldMatrix.data[1][0],
									 _worldMatrix.data[2][0])
						   .length(); // Column 0
		lossyScale.y = math::Vector3(_worldMatrix.data[0][1], _worldMatrix.data[1][1],
									 _worldMatrix.data[2][1])
						   .length(); // Column 1
		lossyScale.z = math::Vector3(_worldMatrix.data[0][2], _worldMatrix.data[1][2],
									 _worldMatrix.data[2][2])
						   .length();

		return lossyScale;
	}

	auto Transform::forward() const -> math::Vector3
	{
		return getRotation() * math::Vector3::forward();
	}

	auto Transform::up() const -> math::Vector3
	{
		return getRotation() * math::Vector3::up();
	}

	auto Transform::right() const -> math::Vector3
	{
		return getRotation() * math::Vector3::right();
	}

	void Transform::setPosition(const math::Vector3& position)
	{
		if (Entity* parent = _entity->getParent())
		{
			math::Matrix4 parentWorld = parent->transform._worldMatrix;
			_localPos = parentWorld.inverse() * position;
		}
		else
		{
			_localPos = position;
		}
		_markDirty();
	}

	void Transform::setRotation(const math::Quaternion& rotation)
	{
		if (Entity* parent = _entity->getParent())
		{
			math::Quaternion parentRot = parent->transform.getRotation();
			_localRot = parentRot.inverse() * rotation;
		}
		else
		{
			_localRot = rotation;
		}
		_markDirty();
	}

	void Transform::translate(const math::Vector3& translation, bool worldSpace)
	{
		if (worldSpace)
		{
			setPosition(getPosition() + translation);
		}
		else
		{
			setLocalPosition(_localPos + _localRot * translation);
		}
	}

	void Transform::rotate(const math::Vector3& eulerAngles, bool worldSpace)
	{
		rotate(eulerAngles.x, eulerAngles.y, eulerAngles.z, worldSpace);
	}

	void Transform::rotate(float xAngle, float yAngle, float zAngle, bool worldSpace)
	{
		math::Quaternion rot = math::Quaternion::fromEuler({xAngle, yAngle, zAngle});
		if (worldSpace)
		{
			setRotation(rot * getRotation());
		}
		else
		{
			setLocalRotation(_localRot * rot);
		}
	}

	void Transform::lookAt(math::Vector3 target, const math::Vector3& worldUp)
	{
		math::Vector3 position = getPosition();
		math::Vector3 forward = (target - position).normalized();
		math::Vector3 right = math::Vector3::cross(worldUp, forward).normalized();
		math::Vector3 up = math::Vector3::cross(forward, right);
		setRotation(math::Quaternion::lookRotation(forward, up));
	}

	// Transformation utilities
	auto Transform::transformDirection(const math::Vector3& direction) const
		-> math::Vector3
	{
		return getRotation() * direction; // Rotation only (no scale/translation)
	}

	auto Transform::transformPoint(const math::Vector3& point) -> math::Vector3
	{
		return _worldMatrix * point; // Full transformation
	}

	auto Transform::inverseTransformDirection(const math::Vector3& direction) const
		-> math::Vector3
	{
		return getRotation().inverse() * direction;
	}

	auto Transform::inverseTransformPoint(const math::Vector3& point) -> math::Vector3
	{
		return _worldMatrix.inverse() * point;
	}

	// Dirty handling
	void Transform::_markDirty()
	{
		if (_dirty)
		{
			return;
		}
		_dirty = true;
		// Propagate to children
		for (Entity* child : _entity->getChildren())
		{
			child->transform._markDirty();
		}
	}

	void Transform::_recomputeWorldMatrix()
	{
		if (!_dirty)
		{
			return;
		}

		_worldMatrix = getParentWorldMatrix() * getLocalMatrix();
		_cachedWorldPos = math::Vector3(_worldMatrix.data[0][3], _worldMatrix.data[1][3],
										_worldMatrix.data[2][3]);
		_cachedWorldRot = _worldMatrix.getRotation();
		_dirty = false;
	}

	auto Transform::getLocalMatrix() -> math::Matrix4
	{
		auto translation = math::Matrix4::translation(_localPos);
		auto rotation = math::Matrix4::rotation(_localRot);
		auto scaling = math::Matrix4::scaling(_localScale);

		return math::Matrix4::multiply({translation, rotation, scaling});
	}

	auto Transform::getParentWorldMatrix() -> math::Matrix4
	{
		if (!_entity->isOrphan())
		{
			return _entity->getParent()->transform.getWorldMatrix();
		}
		return {};
	}

	auto Transform::getWorldMatrix() -> math::Matrix4
	{
		return _worldMatrix;
	}

	[[nodiscard]] auto Transform::getLocalEulerAngles() const -> math::Vector3
	{
		return _localRot.toEuler();
	}

	void Transform::setLocalEulerAngles(const math::Vector3& rotation)
	{
		_localRot = math::Quaternion::fromEuler(rotation);
		_markDirty();
	}

	[[nodiscard]] auto Transform::getEulerAngles() const -> math::Vector3
	{
		return _cachedWorldRot.toEuler();
	}

	void Transform::setEulerAngles(const math::Vector3& rotation)
	{
		if (Entity* parent = _entity->getParent())
		{
			math::Quaternion parentRot = parent->transform.getRotation();
			_localRot = parentRot.inverse() * math::Quaternion::fromEuler(rotation);
		}
		else
		{
			_localRot = math::Quaternion::fromEuler(rotation);
		}
		_markDirty();
	}
}