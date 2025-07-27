#pragma once
#include <fstream>
#include "core/Entity.h"

namespace utils
{
    enum class CrashType : char
    {
        Signal,
        Exception,
        Unknown
    };

	struct CrashContext
	{
		int code;		  // Signal or exception code
		void* address;	  // Faulting memory address
		void* ip;		  // Instruction pointer (RIP/EIP)
		void* sp;		  // Stack pointer (RSP/ESP)
		unsigned int tid; // Faulting thread ID
        CrashType type;

		// Optional: architecture-specific CPU state snapshot
		// Can be raw pointer to platform-specific context
		void* rawContext;
	};

	class CrashReporter
    {
    public:
		using CrashHandler = void (*)(const CrashContext& ctx);

		static void report(const CrashContext& ctx);
        static void registerHandler();

    private:
		static void printRegisters(const CrashContext& ctx, std::ofstream& crash);
		static void printEntity(core::Entity* entity, std::ofstream& crash, int& level);
	};
}