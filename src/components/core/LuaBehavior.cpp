#include "components/core/LuaBehavior.h"
#include "platform/AssetManager.h"
#include "platform/assets/LuaScript.h"
#include <memory>
#include <utility>

namespace core
{
	LuaBehavior::LuaBehavior(const std::string& script)
	{
        log_info("hi");
		this->script = std::dynamic_pointer_cast<platform::LuaScript>(
			AssetManager::getAsset(script));
	}

	LuaBehavior::LuaBehavior(std::shared_ptr<platform::LuaScript> script)
	{
		this->script = std::move(script);
	}

    void LuaBehavior::start()
    {
        addHotspot("start");
        addHotspot("update");
        
        callHotspot("start");
    }

    void LuaBehavior::update()
    {
		callHotspot("update");
	}

	void LuaBehavior::addHotspot(const std::string& name)
    {
        if (script->hasFunction(name))
        {
            hotSpots[name] = script->table[name];
        }
    }
}