#include "components/graphics/Camera.h"
#include "core/Entity.h"
#include "core/Scene.h"
#include "utils/math/Matrix4.h"
#include "utils/math/Vector2.h"

namespace core
{
	void Camera::update()
	{
		if (getEntity()->getTag() == es_mainCameraTag)
		{
			mainCamera = this;
		}

		if (_dirty)
		{
			_verticalFov = 2.0F * std::atan(sensorSize.y / (2.0F * focalLength));
			_horizontalFov = 2.0F * std::atan(sensorSize.x / (2.0F * focalLength));
			_sensorAspect = sensorSize.x / sensorSize.y;

			auto fovAspect = _calculateFieldOfView();
            auto fovY = fovAspect.x;
            auto sensorAspect = fovAspect.y;

			// apply lens shift
			float top = near * std::tan(fovY / 2.0F) * (1.0F - 2.0F * lensShift.y);
			float bottom = -near * std::tan(fovY / 2.0F) * (1.0F + 2.0F * lensShift.y);
			float right =
				near * sensorAspect * std::tan(fovY / 2.0F) * (1.0F + 2.0F * lensShift.x);
			float left = -near * sensorAspect * std::tan(fovY / 2.0F) *
						 (1.0F - 2.0F * lensShift.x);

			if (projectionType == ProjectionType::Perspective)
			{
				_projection = math::Matrix4::frustum(left, right, bottom, top, near, far);
			}
			else
			{
				_projection = math::Matrix4::ortho(left, right, bottom, top, near, far);
			}
		}
	}

	auto Camera::_calculateFieldOfView() -> math::Vector2
	{
		float screenAspect = viewportSize.x / viewportSize.y;

		float fovY = _verticalFov;
		float aspect = _sensorAspect;

		// Adjust FOV and aspect based on gate fit
		switch (gateFit)
		{
		case GateFit::Horizontal:
			fovY = 2.0F * std::atan(std::tan(_horizontalFov / 2.0F) / screenAspect);
			aspect = screenAspect;
			break;
		case GateFit::Vertical:
			aspect = screenAspect;
			break;
		case GateFit::Fill:
			aspect = (screenAspect >= _sensorAspect) ? screenAspect : _sensorAspect;
			break;
		case GateFit::Overscan:
			aspect = (screenAspect >= _sensorAspect) ? _sensorAspect : screenAspect;
			break;
		case GateFit::None:
			break; // Use sensor aspect
		}

		return {fovY, aspect};
	}

    void Camera::onEnable()
    {

    }

    void Camera::onDisable()
    {

    }

	auto Camera::getProjectionType() const -> ProjectionType
	{
		return projectionType;
	}

	auto Camera::getFocalLength() const -> float
	{
		return focalLength;
	}

	auto Camera::getSensorSize() const -> math::Vector2
	{
		return sensorSize;
	}

	auto Camera::getLensShift() const -> math::Vector2
	{
		return lensShift;
	}

	auto Camera::getNearPlane() const -> float
	{
		return near;
	}

	auto Camera::getFarPlane() const -> float
	{
		return far;
	}

	auto Camera::getGateFit() const -> GateFit
	{
		return gateFit;
	}

	auto Camera::getRenderOrder() const -> int
	{
		return renderOrder;
	}

	auto Camera::getViewportSize() const -> math::Vector2
	{
		return viewportSize;
	}

	void Camera::setProjectionType(ProjectionType type)
	{
		projectionType = type;
		_dirty = true;
	}

	void Camera::setFocalLength(float length)
	{
		focalLength = length;
		_dirty = true;
	}

	void Camera::setSensorSize(math::Vector2 size)
	{
		sensorSize = size;
		_dirty = true;
	}

	void Camera::setLensShift(math::Vector2 shift)
	{
		lensShift = shift;
		_dirty = true;
	}

	void Camera::setNearPlane(float near)
	{
		this->near = near;
		_dirty = true;
	}

	void Camera::setFarPlane(float far)
	{
		this->far = far;
		_dirty = true;
	}

	void Camera::setGateFit(GateFit fit)
	{
		gateFit = fit;
		_dirty = true;
	}

	void Camera::setRenderOrder(int order)
	{
		renderOrder = order;
		getEntity()->getScene()->requestCameraReorder();
	}

	auto Camera::getSensorAspect() const -> float
	{
		return _sensorAspect;
	}

	void Camera::setViewportSize(math::Vector2 viewport)
	{
		viewportSize = viewport;
		_dirty = true;
	}
}