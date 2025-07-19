#include "platform/assets/LuaScript.h"
#include "components/core/LuaScriptEngine.h"

namespace platform
{
	LuaScript::LuaScript()
	{
		env = sol::environment(core::LuaScriptEngine::state, sol::create,
							   core::LuaScriptEngine::state.globals());
		table = sol::table(core::LuaScriptEngine::state);
	}

	auto LuaScript::has(const std::string& name) -> bool
	{
		return table[name].valid();
	}

	auto LuaScript::hasFunction(const std::string& name) -> bool
	{
		return has(name) && table[name].is<sol::function>();
	}

	auto LuaScriptProcessor::getDefaultAsset() -> std::shared_ptr<core::Asset>
	{
		auto script = std::make_shared<LuaScript>();
		script->env = sol::environment(core::LuaScriptEngine::state, sol::create,
									   core::LuaScriptEngine::state.globals());
		return script;
	}

	auto LuaScriptProcessor::canLoad(const std::string& extension) -> bool
	{
		return extension == ".lua";
	}

	auto LuaScriptProcessor::load(char* data, unsigned long size)
		-> std::shared_ptr<core::Asset>
	{
		auto script = std::make_shared<LuaScript>();
		script->env = sol::environment(core::LuaScriptEngine::state, sol::create,
									   core::LuaScriptEngine::state.globals());
		script->table = core::LuaScriptEngine::state.safe_script(std::string_view(data, size),
													   script->env);
        return script;
	}
}