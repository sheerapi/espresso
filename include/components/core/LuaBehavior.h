#pragma once
#include "core/Component.h"
#include "platform/assets/LuaScript.h"
#include "sol/unsafe_function_result.hpp"
#include <memory>
#include <unordered_map>

namespace core
{
    class LuaBehavior : public Component
    {
    public:
        LuaBehavior(const std::string& script);
        LuaBehavior(std::shared_ptr<platform::LuaScript> script);
        
        void start() override;
        void update() override;

        template<typename... Args>
        auto call(const std::string& name, Args&& ...args) -> sol::unsafe_function_result
        {
            if (hotSpots.count(name) != 0)
            {
                return hotSpots[name].call(std::forward<Args>(args)...);
            }

			if (!script->hasFunction(name))
			{
				log_error("function %s not found", name.c_str());
			}

			return script->env[name].call(std::forward<Args>(args)...);
		}

    protected:
        std::shared_ptr<platform::LuaScript> script;
        std::unordered_map<std::string, sol::function> hotSpots;

        void addHotspot(const std::string& name);

		template <typename... Args>
		auto callHotspot(const std::string& name, Args&&... args)
			-> sol::unsafe_function_result
		{
			if (hotSpots.count(name) == 0)
			{
				return {};
			}

			return hotSpots[name].call(std::forward<Args>(args)...);
		};
    };
}