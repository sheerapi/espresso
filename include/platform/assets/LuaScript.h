#pragma once
#include "platform/AssetManager.h"
#include "sol/environment.hpp"

namespace platform
{
    class LuaScript : public core::Asset
    {
    public:
        sol::environment env;

        auto has(const std::string& name) -> bool;
		auto hasFunction(const std::string& name) -> bool;
	};

    class LuaScriptProcessor : public core::AssetProcessor
    {
    public:
        auto getDefaultAsset() -> std::shared_ptr<core::Asset> override;
        auto canLoad(const std::string &extension) -> bool override;
        auto load(char *data, unsigned long size) -> std::shared_ptr<core::Asset> override;
        auto deferredLoad() -> bool override
        {
            return false;
        }
    };
}