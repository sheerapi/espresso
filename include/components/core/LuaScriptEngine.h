#pragma once
#include "sol/state.hpp"

namespace core
{
    class LuaScriptEngine
    {
    public:
        static void init();
        static void shutdown();

        inline static sol::state state;
    };
}