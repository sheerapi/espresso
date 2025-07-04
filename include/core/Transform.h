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
		void update();
		
		// Local properties
		[[nodiscard]] auto getLocalPosition() const -> math::Vector3
		{
			return _localPos;
		}

		void setLocalPosition(const math::Vector3& position);

		[[nodiscard]] auto getLocalRotation() const -> math::Quaternion
		{
			return _localRot;
		}
		
		void setLocalRotation(const math::Quaternion& rotation);

		[[nodiscard]] auto getLocalScale() const -> math::Vector3
		{
			return _localScale;
		}
		void setLocalScale(const math::Vector3& scale);

		// World properties
		[[nodiscard]] auto getPosition() const -> math::Vector3;
		void setPosition(const math::Vector3& position);

		[[nodiscard]] auto getRotation() const -> math::Quaternion;
		void setRotation(const math::Quaternion& rotation);

		[[nodiscard]] auto getLossyScale()
			const -> math::Vector3; // "Lossy" because non-uniform scaling complicates extraction

		// Direction vectors
		[[nodiscard]] auto forward() const -> math::Vector3;
		[[nodiscard]] auto up() const -> math::Vector3;
		[[nodiscard]] auto right() const -> math::Vector3;

		// Transformation functions
		void translate(const math::Vector3& translation, bool worldSpace = false);
		void rotate(const math::Vector3& eulerAngles, bool worldSpace = false);
		void rotate(float xAngle, float yAngle, float zAngle, bool worldSpace = false);
		void lookAt(math::Vector3 target,
					const math::Vector3& worldUp = {0, 1, 0});

		[[nodiscard]] auto transformDirection(const math::Vector3& direction) const -> math::Vector3;
		[[nodiscard]] auto transformPoint(const math::Vector3& point) -> math::Vector3;
		[[nodiscard]] auto inverseTransformDirection(const math::Vector3& direction) const -> math::Vector3;
		[[nodiscard]] auto inverseTransformPoint(const math::Vector3& point) -> math::Vector3;

	private:
		math::Vector3 _localPos;
		math::Quaternion _localRot;
		math::Vector3 _localScale{1, 1, 1};
		core::Entity* _entity = nullptr;

		bool _dirty{true};
		math::Matrix4 _worldMatrix;
		math::Vector3 _cachedWorldPos;
		math::Quaternion _cachedWorldRot;

		void _markDirty();
		void _recomputeWorldMatrix();

		auto _getLocalMatrix() -> math::Matrix4;
		auto _getParentWorldMatrix() -> math::Matrix4;
		auto _getWorldMatrix() -> math::Matrix4;

		friend class core::Entity;
	};
}