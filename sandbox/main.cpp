#include "core/Application.h"
#include "core/EntryPoint.h"
#include "platform/AssetManager.h"

class SandboxApp : public core::Application
{
public:
    void init() override
    {
        appName = "sandbox";
        core::AssetManager::setAssetDirectory("../assets");
	}
};

es_createApp(SandboxApp);