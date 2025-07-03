#include "core/Application.h"
#include "core/Component.h"
#include "core/Entity.h"
#include "core/EntryPoint.h"
#include "core/Scene.h"
#include "platform/AssetManager.h"
#include <memory>

class TestProcessor : public core::AssetProcessor
{
public:
    auto canLoad(const std::string &extension) -> bool override
    {
        return true;
    }

    auto load(char *data, unsigned long size) -> std::shared_ptr<core::Asset> override
    {
        log_info("receiving %d bytes of data", size);
        return std::make_shared<core::Asset>();
    }
};

class SandboxApp : public core::Application
{
public:
    void init() override
    {
        appName = "sandbox";
        core::AssetManager::loadFolder("../assets");
        core::AssetManager::loadBundle("assets");
        core::AssetManager::registerProcessor<TestProcessor>();
        core::AssetManager::getAsset("man_computer.png");
	}
};

es_createApp(SandboxApp);