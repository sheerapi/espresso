#include "core/Application.h"
#include "core/Component.h"
#include "core/Entity.h"
#include "core/EntryPoint.h"

class SandboxApp : public core::Application
{
public:
    void init() override
    {
        appName = "sandbox";
	}
};

es_createApp(SandboxApp);