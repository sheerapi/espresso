#include "utils/CrashReporter.h"
#include "components/core/LuaScriptEngine.h"
#include "core/Application.h"
#include "core/Scene.h"
#include "core/jobs/JobManager.h"
#include "magic_enum/magic_enum.hpp"
#include "platform/AssetManager.h"
#include "platform/StackTrace.h"
#include "platform/ThreadManager.h"
#include "utils/StringUtils.h"
#include <ctime>
#include <filesystem>

#ifdef __unix
#	include <csignal>
#	include <cstdlib>
#	include <sys/syscall.h>
#	include <dirent.h>
#	include <sys/types.h>
#else
#	include <windows.h>
#endif

namespace utils
{
	auto formatRegister(const std::string& name, uint64_t value) -> std::string;
	auto formatRegister(const std::string& name, uint32_t value) -> std::string;

	void CrashReporter::report(const CrashContext& ctx)
	{
		auto* envInfo = core::Application::main->getEnvironmentInfo();
		auto* frameMemoryPool = core::Application::main->getFrameMemoryPool();

		auto timestamp = std::time(nullptr);
		auto* date = localtime(&timestamp);

		char name[24];
		std::strftime(name, sizeof(name), "%d-%m-%y-%H.%M.txt", date);

		std::ofstream crash(std::filesystem::path(envInfo->paths.logPath) / name);

		crash << "==== Crash Report (" << core::Application::main->getAppInfo().appId
			  << ") ====\n";
		crash << "we ran into an error during execution, sorry about that.\n\n";

		char timeBuffer[20];
		std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", date);
		crash << "Time: " << timeBuffer << "\n\n";

		crash << "System information:\n";
		crash << "Running Espresso " << envInfo->engineVersion.major << "."
			  << envInfo->engineVersion.minor << "." << envInfo->engineVersion.patch
			  << (envInfo->engineVersion.metadata.empty()
					  ? ""
					  : "-" + envInfo->engineVersion.metadata);

		crash << " on " << envInfo->system.platform << " " << envInfo->system.osVersion
			  << " (" << envInfo->system.architecture << ")\n";

		std::string subscriptionState = "undersubscribed";

		if (core::jobs::JobManager::threadCount() +
				platform::ThreadManager::threadCount() >
			envInfo->system.cpuCores)
		{
			subscriptionState = "oversubscribed";
		}

		crash << "Running on " << envInfo->system.cpuBrand << "\n";

		crash << "Spawned "
			  << (core::jobs::JobManager::threadCount() +
				  platform::ThreadManager::threadCount())
			  << " threads out of " << envInfo->system.cpuCores << " cores // "
			  << subscriptionState << "\n";

		crash << "Out of " << envInfo->system.totalMemoryMB << "MB of memory, "
			  << envInfo->system.availableMemoryMB << "MB are available. Used "
			  << ((float)frameMemoryPool->getBytesUsed() / (1024 * 1024))
			  << "MB per frame ("
			  << (1.F - ((float)envInfo->system.availableMemoryMB /
						 (float)envInfo->system.totalMemoryMB)) *
					 100.F
			  << "% used, "
			  << (((float)frameMemoryPool->getBytesUsed() / (1024 * 1024) /
				   (float)envInfo->system.totalMemoryMB) *
				  100.F)
			  << "% per frame)\n";

		auto endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());

		auto uptime = endTime - core::internals::startTime;

		auto uptimeHours = std::chrono::duration_cast<std::chrono::hours>(uptime);
		uptime -= uptimeHours;

		auto uptimeMinutes = std::chrono::duration_cast<std::chrono::minutes>(uptime);
		uptime -= uptimeMinutes;

		auto uptimeSeconds = std::chrono::duration_cast<std::chrono::seconds>(uptime);
		uptime -= uptimeSeconds;

		auto uptimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(uptime);

		crash << "Ran for " << uptimeHours.count() << " hours, " << uptimeMinutes.count()
			  << " minutes, " << uptimeSeconds.count() << " seconds and "
			  << uptimeMs.count() << "ms before crashing"
			  << (uptimeSeconds.count() < 2 ? " // not even 2 seconds? that was fast\n"
											: "\n")
			  << "\n";

		const auto* signalPrefix = (ctx.type == CrashType::Signal ? "a" : "an");
		crash << "Crashed due to " << signalPrefix << (ctx.code == 0 ? " unknown " : " ")
			  << stringToLower(std::string(magic_enum::enum_name(ctx.type)))
			  << " with code " << ctx.code << " // " << platform::signalToString(ctx.code)
			  << "\n";

		crash << "Executing as " << magic_enum::enum_name(envInfo->executionMode)
			  << " in " << magic_enum::enum_name(envInfo->buildConfig) << " profile\n";

		crash << "Ran with ";

		switch (envInfo->commandLineArgs.size())
		{
		case 0:
			crash << "no arguments";
			break;

		case 1:
			crash << "1 argument";
			break;

		default:
			crash << envInfo->commandLineArgs.size() << " arguments";
			break;
		}

		if (!envInfo->commandLineArgs.empty())
		{
			crash << ": ";

			for (auto& arg : envInfo->commandLineArgs)
			{
				crash << arg << " ";
			}
		}

		crash << "\n\n";

		crash << "--- Faulty Thread ---\n\n";
		if (ctx.code != 0xDEAD)
		{
			crash << "Exception code:   " << "0x" << std::hex << ctx.code << "\n";
			crash << "Faulting address: " << "0x" << std::hex << ctx.address << "\n";
			crash << "Instruction ptr:  " << std::hex << ctx.ip << "\n";
			crash << "Registers:\n\n";
			printRegisters(ctx, crash);

			crash << "\nStack trace (most recent call first):\n";
		}
		else
		{
			crash << "Exception thrown from within C++ context. I cannot provide any "
					 "insights";
		}

		std::unordered_map<std::thread::id, std::vector<platform::StackTrace::Frame>>
			threadStacks;
		platform::StackTrace::captureAllThreads(threadStacks);

		for (auto& thread : threadStacks)
		{
			crash << platform::StackTrace::format(thread.second);
		}

		luaL_traceback(core::LuaScriptEngine::state.lua_state(),
					   core::LuaScriptEngine::state.lua_state(), nullptr, 1);

		const char* tb = lua_tostring(core::LuaScriptEngine::state.lua_state(), -1);
		std::string luaStack = tb ? tb : "";
		lua_pop(core::LuaScriptEngine::state.lua_state(), 1);

		crash << "\nlua " << luaStack << "\n";

		crash << "\n--- Miscellaneous ---\n";
		crash << "// useful stuff\n\n";

		crash << "logger output up to this point:\n";
		std::ifstream log(std::filesystem::path(envInfo->paths.logPath) / "log.txt");

		crash << log.rdbuf();
		log.close();

		auto assets = core::AssetManager::getLoadedAssets();
		crash << "\n"
			  << (assets.size() == 0 ? "no " : "") << "assets loaded"
			  << (assets.size() == 0 ? "" : ":") << "\n";

		for (auto& asset : assets)
		{
			crash << "  - " << asset.first << " (id=" << asset.second->getID() << ", at "
				  << std::hex << asset.second.get() << ")\n";
		}

		crash << "\nscene hierarchy (" << core::Scene::currentScene->getName() << "):\n";

		for (auto* entity : core::Scene::currentScene->getEntities())
		{
			int level{0};
			printEntity(entity, crash, level);
		}
	}

	void CrashReporter::printEntity(core::Entity* entity, std::ofstream& crash,
									int& level)
	{
		if (level >= 4)
		{
			return;
		}

		std::string tabs;
		for (auto depth = 0; depth < level; depth++)
		{
			tabs += "\t";
		}

		crash << tabs << "- " << entity->getName() << "(tag=" << (int)entity->getTag()
			  << ", id=" << entity->getID() << ", "
			  << (entity->isActive() ? "active" : "inactive") << "/"
			  << (entity->isVisible() ? "visible" : "invisible") << ")\n";

		crash << tabs << "\t[";

		auto components = entity->getComponents();
		for (size_t i = 0; i < components.size(); i++)
		{
			crash << demangle(typeid(*components[i]).name())
				  << (i == components.size() - 1 ? "" : ", ");
		}

		crash << "]\n";

		level++;

		for (auto* child : entity->getChildren())
		{
			printEntity(child, crash, level);
		}
	}

	void CrashReporter::printRegisters(const CrashContext& ctx, std::ofstream& crash)
	{
#if defined(__x86_64__) || defined(_M_X64)
		constexpr int RegisterCount = 16;
		const char* regNames[16] = {"RSP", "RBP", "RAX", "RBX", "RCX", "RDX",
									"RSI", "RDI", "R08", "R09", "R10", "R11",
									"R12", "R13", "R14", "R15"};

#elif defined(__i386__) || defined(_M_IX86)
		constexpr int RegisterCount = 9;
		const char* regNames[9] = {"EIP", "ESP", "EBP", "EAX", "EBX",
								   "ECX", "EDX", "ESI", "EDI"};
#endif

#ifdef __unix
		auto* uctx = (ucontext_t*)ctx.rawContext;
		auto& regs = uctx->uc_mcontext.gregs;

#	if defined(__x86_64__)
		uint64_t regVals[16] = {
			(uint64_t)regs[REG_RSP], (uint64_t)regs[REG_RBP], (uint64_t)regs[REG_RAX],
			(uint64_t)regs[REG_RBX], (uint64_t)regs[REG_RCX], (uint64_t)regs[REG_RDX],
			(uint64_t)regs[REG_RSI], (uint64_t)regs[REG_RDI], (uint64_t)regs[REG_R8],
			(uint64_t)regs[REG_R9],	 (uint64_t)regs[REG_R10], (uint64_t)regs[REG_R11],
			(uint64_t)regs[REG_R12], (uint64_t)regs[REG_R13], (uint64_t)regs[REG_R14],
			(uint64_t)regs[REG_R15]};
#	elif defined(__i386__)
		uint32_t regVals[9] = {
			(uint32_t)regs[REG_EIP], (uint32_t)regs[REG_ESP], (uint32_t)regs[REG_EBP],
			(uint32_t)regs[REG_EAX], (uint32_t)regs[REG_EBX], (uint32_t)regs[REG_ECX],
			(uint32_t)regs[REG_EDX], (uint32_t)regs[REG_ESI], (uint32_t)regs[REG_EDI]};
#	endif

#else
		auto* c = (CONTEXT*)ctx.rawContext;
#	if defined(_M_X64)
		uint64_t regVals[16] = {
			(uint64_t)c->Rsp, (uint64_t)c->Rbp, (uint64_t)c->Rax, (uint64_t)c->Rbx,
			(uint64_t)c->Rcx, (uint64_t)c->Rdx, (uint64_t)c->Rsi, (uint64_t)c->Rdi,
			(uint64_t)c->R8,  (uint64_t)c->R9,	(uint64_t)c->R10, (uint64_t)c->R11,
			(uint64_t)c->R12, (uint64_t)c->R13, (uint64_t)c->R14, (uint64_t)c->R15};
#	elif defined(_M_IX86)
		uint32_t regVals[9] = {(uint32_t)c->Eip, (uint32_t)c->Esp, (uint32_t)c->Erbp,
							   (uint32_t)c->Eax, (uint32_t)c->Ebx, (uint32_t)c->Ecx,
							   (uint32_t)c->Edx, (uint32_t)c->Esi, (uint32_t)c->Edi};
#	endif
#endif

		for (int i = 0; i < RegisterCount; i += RegisterCount % 6)
		{
			for (int j = 0; j < RegisterCount % 6; ++j)
			{
				crash << formatRegister(regNames[i + j], regVals[i + j]) << "\t";
			}
			crash << "\n";
		}
	}

	auto formatRegister(const std::string& name, uint64_t value) -> std::string
	{
		std::ostringstream oss;
		oss << std::left << std::setw(4) << name << ": 0x" << std::setfill('0')
			<< std::setw(16) << std::hex << value;
		return oss.str();
	}

	auto formatRegister(const std::string& name, uint32_t value) -> std::string
	{
		std::ostringstream oss;
		oss << std::left << std::setw(4) << name << ": 0x" << std::setfill('0')
			<< std::setw(8) << std::hex << value;
		return oss.str();
	}

#ifdef __unix
	void signalHandler(int sig, siginfo_t* info, void* ucontextRaw);
#else
	LONG WINAPI sehHandler(EXCEPTION_POINTERS* ep);
#endif

	void CrashReporter::registerHandler()
	{
#ifdef __unix
		struct sigaction sa{};
		sa.sa_sigaction = signalHandler;
		sa.sa_flags = SA_SIGINFO;

		sigaction(SIGSEGV, &sa, nullptr);
		sigaction(SIGILL, &sa, nullptr);
		sigaction(SIGFPE, &sa, nullptr);
		sigaction(SIGABRT, &sa, nullptr);
		sigaction(SIGBUS, &sa, nullptr);
#else
		SetUnhandledExceptionFilter(sehHandler);
#endif

		std::set_terminate(
			[]()
			{
				CrashContext ctx;
				ctx.code = 0xDEAD;
#ifdef __unix
				ctx.tid = syscall(SYS_gettid);
#else
				ctx.tid = GetCurrentThreadId();
#endif

				ctx.ip = nullptr;
				ctx.sp = nullptr;

				ctx.address = nullptr;
				ctx.rawContext = nullptr;

				CrashReporter::report(ctx);
				std::quick_exit(1);
			});
	}

#ifdef __unix
	void signalHandler(int sig, siginfo_t* info, void* ucontextRaw)
	{
		CrashContext ctx = {};
		ctx.code = sig;
		ctx.address = info->si_addr;
		ctx.tid = syscall(SYS_gettid);
		ctx.rawContext = ucontextRaw;

#	if defined(__x86_64__)
		ucontext_t* uctx = (ucontext_t*)ucontextRaw;
		ctx.ip = (void*)uctx->uc_mcontext.gregs[REG_RIP];
		ctx.sp = (void*)uctx->uc_mcontext.gregs[REG_RSP];
#	elif defined(__i386__)
		ctx.ip = (void*)uctx->uc_mcontext.gregs[REG_EIP];
		ctx.sp = (void*)uctx->uc_mcontext.gregs[REG_ESP];
#	endif

		CrashReporter::report(ctx);

		std::quick_exit(sig);
	}
#endif

#ifdef _WIN32
	LONG WINAPI sehHandler(EXCEPTION_POINTERS* ep)
	{
		CrashContext ctx = {};
		ctx.code = ep->ExceptionRecord->ExceptionCode;
		ctx.address = ep->ExceptionRecord->ExceptionAddress;
		ctx.rawContext = ep->ContextRecord;
		ctx.tid = GetCurrentThreadId();

#	if defined(_M_X64)
		ctx.ip = (void*)ep->ContextRecord->Rip;
		ctx.sp = (void*)ep->ContextRecord->Rsp;
#	elif defined(_M_IX86)
		ctx.ip = (void*)ep->ContextRecord->Eip;
		ctx.sp = (void*)ep->ContextRecord->Esp;
#	endif

		CrashReporter::report(ctx);

		return EXCEPTION_EXECUTE_HANDLER;
	}
#endif
}