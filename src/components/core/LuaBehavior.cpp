#include "components/core/LuaBehavior.h"
#include "platform/AssetManager.h"
#include "platform/assets/LuaScript.h"
#include <memory>
#include "magic_enum/magic_enum.hpp"
#include <utility>

namespace core
{
	LuaBehavior::LuaBehavior(const std::string& script)
	{
		this->script = std::dynamic_pointer_cast<platform::LuaScript>(
			AssetManager::getAsset(script));

		if (this->script == nullptr)
		{
			log_warn("assigned a nil script to entity");
			this->script = std::make_unique<platform::LuaScript>();
		}
	}

	LuaBehavior::LuaBehavior(std::shared_ptr<platform::LuaScript> script)
	{
		this->script = std::move(script);
	}

	void LuaBehavior::start()
	{
		for (auto& member : script->table)
		{
			log_info("member %s of type %s", member.first.as<std::string>().c_str(),
					 std::string(magic_enum::enum_name(member.second.get_type())).c_str());
		}

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