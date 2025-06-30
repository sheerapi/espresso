#include "core/Application.h"
#include "core/EntryPoint.h"
#include "core/Scene.h"

class SandboxApp : public core::Application
{
public:
    void init() override
    {
        appName = "sandbox";
        core::Scene scene("Test Scene");
	}
};

es_createApp(SandboxApp);