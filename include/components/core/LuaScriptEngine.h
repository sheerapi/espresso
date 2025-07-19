#pragma once
#include "sol/state.hpp"

namespace core
{
	class LuaScriptEngine
	{
	public:
		static void init();
        static void update();
		static void shutdown();
        static void bindApi();

		inline static sol::state state;

	private:
		static auto convertFormat(const std::string& fmt, std::vector<int>& argOrder)
			-> std::string;
		static auto luaLog(sol::variadic_args args) -> std::string;

        inline static int gcTick{0};
	};
}