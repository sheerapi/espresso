#include "components/core/LuaScriptEngine.h"
#include "core/log.h"
#include "sol/variadic_args.hpp"

namespace core
{
	void LuaScriptEngine::bindApi()
	{
		
	}

	void LuaScriptEngine::init()
	{
		state = sol::state();
		state.open_libraries(sol::lib::base, sol::lib::math, sol::lib::utf8,
							 sol::lib::string, sol::lib::table, sol::lib::coroutine,
							 sol::lib::package);

		bindApi();
	}

	void LuaScriptEngine::update()
	{
		if (++gcTick % 60 == 0)
		{
			state.collect_garbage();
		}
	}

	void LuaScriptEngine::shutdown()
	{
	}

	auto LuaScriptEngine::luaLog(sol::variadic_args args) -> std::string
	{
		if (args.size() == 0)
		{
			return "";
		}

		std::string fmt_string = args[0].get<std::string>();

		// Parse format string to printf version + argument order
		std::vector<int> arg_order;
		std::string printf_fmt = convertFormat(fmt_string, arg_order);

		// Build stringified arguments
		std::vector<std::string> raw_args;
		for (size_t i = 1; i < args.size(); ++i)
		{
			std::ostringstream ss;
			sol::object obj = args[i];
			switch (obj.get_type())
			{
			case sol::type::string:
				ss << obj.as<std::string>();
				break;
			case sol::type::number:
				ss << obj.as<double>();
				break;
			case sol::type::boolean:
				ss << (obj.as<bool>() ? "true" : "false");
				break;
			case sol::type::nil:
				ss << "nil";
				break;
			default:
				ss << "[unknown]";
				break;
			}
			raw_args.push_back(ss.str());
		}

		// Build args in order
		const char* fmt_args[6] = {"", "", "", "", "", ""};
		for (size_t i = 0; i < arg_order.size() && i < 6; ++i)
		{
			int idx = arg_order[i];
			if (idx >= 0 && static_cast<size_t>(idx) < raw_args.size())
			{
				fmt_args[i] = raw_args[idx].c_str();
			}
		}

		// Format final string
		char buffer[1024];
		std::snprintf(buffer, sizeof(buffer), printf_fmt.c_str(), fmt_args[0],
					  fmt_args[1], fmt_args[2], fmt_args[3], fmt_args[4], fmt_args[5]);

		return std::string(buffer);
	}

	auto LuaScriptEngine::convertFormat(const std::string& fmt,
										std::vector<int>& argOrder) -> std::string
	{
		std::string out;
		size_t last = 0;
		int implicit_index = 0;

		for (size_t i = 0; i < fmt.size(); ++i)
		{
			if (fmt[i] == '{')
			{
				out += fmt.substr(last, i - last); // append text before '{'
				size_t end = fmt.find('}', i);
				if (end == std::string::npos)
				{
					log_error("unmatched '{' in format string");
					return "";
				}

				std::string index_str = fmt.substr(i + 1, end - i - 1);
				int index;

				if (index_str.empty())
				{
					index = implicit_index++;
				}
				else
				{
					index = std::stoi(index_str);
				}

				argOrder.push_back(index);
				out += "%s";
				i = end; // move past '}'
				last = i + 1;
			}
		}
		out += fmt.substr(last); // append remaining text
		return out;
	}
}