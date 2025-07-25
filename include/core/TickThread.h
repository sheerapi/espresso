#pragma once
#include "components/core/LuaScriptEngine.h"
#include "platform/Thread.h"

namespace core
{
	class TickThread : public platform::Thread
	{
	public:
		void init() override;
		void shutdown() override;
		void update() override;
	};
}