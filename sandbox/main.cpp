#include "core/Application.h"
#include "core/Component.h"
#include "core/Entity.h"
#include "core/EntryPoint.h"
#include "utils/math/Vector2.h"

class SandboxApp : public core::Application
{
public:
	void init() override
	{
		appName = "sandbox";
		math::Vector2 vec{12, 8};
        vec += math::Vector2{1, 1};
        log_info("%f %f", vec.x, vec.y);
	}
};

es_createApp(SandboxApp);