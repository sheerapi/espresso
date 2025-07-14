#include "components/core/LuaScriptEngine.h"

namespace core
{
	void LuaScriptEngine::init()
	{
		state = sol::state();
		state.open_libraries(sol::lib::base, sol::lib::math, sol::lib::utf8,
							 sol::lib::string, sol::lib::table, sol::lib::coroutine,
							 sol::lib::package);
	}

	void LuaScriptEngine::shutdown()
	{
	}
}