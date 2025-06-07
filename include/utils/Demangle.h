#pragma once
#include "core/log.h"
#include <string>
#include <typeinfo>

/**
 * @brief Tries to demangle a name, used internally
 *
 * @param name Name to demangle, usually by typeid().name()
 * @return std::string Demangled name, if using GCC/Clang/MSVC, otherwise the mangled one
 */
auto demangle(const char* name) -> std::string;

#define es_type(type) demangle(typeid(type).name())

auto getNamespaceFreeName(const std::string& name) -> std::string;

template <typename T1, typename T2> auto typeCheck() -> bool
{
	if (!std::is_base_of<T1, T2>())
	{
		log_error("expected %s, got %s", es_type(T1).c_str(), es_type(T2).c_str());
		return false;
	}

	return true;
}