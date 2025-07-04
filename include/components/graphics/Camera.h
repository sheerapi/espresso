#pragma once
#include "core/Component.h"
#include "utils/math/Matrix4.h"
#include "utils/math/Vector2.h"
#include <cstdint>

#define es_mainCameraTag 1

namespace core
{
	enum class GateFit : uint8_t
	{
		Vertical,
		Horizontal,
		Fill,
		Overscan,
		None
	};

	enum class ProjectionType : bool
	{
		Perspective,
		Orthographic
	};

	class Camera : public Component
	{
	public:
		inline static Camera* mainCamera{nullptr};

		void update() override;
		void onEnable() override;
		void onDisable() override;

		auto getProjectionType() const -> ProjectionType;
		auto getFocalLength() const -> float;
		auto getSensorSize() const -> math::Vector2;
		auto getLensShift() const -> math::Vector2;

        auto getNearPlane() const -> float;
        auto getFarPlane() const -> float;

        auto getGateFit() const -> GateFit;
        auto getRenderOrder() const -> int;
        auto getViewportSize() const -> math::Vector2;

        void setProjectionType(ProjectionType type);
        void setFocalLength(float length);
        void setSensorSize(math::Vector2 size);
        void setLensShift(math::Vector2 shift);
        void setNearPlane(float near);
        void setFarPlane(float far);

        void setGateFit(GateFit fit);
        void setRenderOrder(int order);

        void setViewportSize(math::Vector2 viewport);

		auto getSensorAspect() const -> float;

	protected:
		ProjectionType projectionType{ProjectionType::Perspective};

		float focalLength = 35.F;
		math::Vector2 sensorSize{36.F, 24.F};
		math::Vector2 lensShift;
        math::Vector2 viewportSize;

		float near = 0.1F;
		float far = 1000.F;

		GateFit gateFit{GateFit::Horizontal};

		int renderOrder{0};

	private:
		float _verticalFov;
		float _horizontalFov;
		float _sensorAspect;
		bool _dirty{true};

		math::Matrix4 _projection;
		math::Matrix4 _view;

		// calculates the new field of view and aspect
        // based on gate fit
        // x is vertical fov, y is sensor aspect
        auto _calculateFieldOfView() -> math::Vector2;
	};
}