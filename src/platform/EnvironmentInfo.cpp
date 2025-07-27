#pragma once
#include "platform/EnvironmentInfo.h"
#include "core/Application.h"
#include "core/log.h"
#include "utils/CrashReporter.h"
#include "utils/PerformanceTimer.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

#if defined(_WIN32)
#	include <Windows.h>
#	include <winnt.h>
#elif defined(__APPLE__)
#	include <sys/utsname.h>
#	include <mach-o/dyld.h>
#	include <signal.h>
#elif defined(__linux)
#	include <fstream>
#	include <sys/sysinfo.h>
#	include <unistd.h>
#	include <signal.h>
#endif

namespace platform
{
	auto getEnvironmentVariables() -> std::unordered_map<std::string, std::string>;
	auto getPlatform() -> std::string;
	auto getArchitecture() -> std::string;
	auto getOSVersion() -> std::string;
	auto getCPUBrand() -> std::string;
	auto getCoreCount() -> uint32_t;
	auto getTotalMemoryMB() -> uint64_t;
	auto getAvailableMemoryMB() -> uint64_t;
	auto getExecutablePath() -> std::string;
	auto getDirectory(const std::string& path) -> std::string;
	auto getTempDirectory() -> std::string;
	auto getUserDataPath(const std::string& org, const std::string& appName)
		-> std::string;
	auto getCachePath(const std::string& org, const std::string& appName) -> std::string;
	auto getLogPath(const std::string& org, const std::string& appName) -> std::string;

	void EnvironmentInfo::initialize(int argc, const char** argv)
	{
		es_stopwatchNamed("init env info");

		for (size_t i = 1; i < argc; i++)
		{
			commandLineArgs.emplace_back(argv[i]);
		}

#ifdef EDITOR
		executionMode = ExecutionMode::Editor;
#endif

#if defined(DEBUG) && !defined(__OPTIMIZE__)
		buildConfig = BuildConfiguration::Debug;
#elif defined(DEBUG) && defined(__OPTIMIZE__)
		buildConfig = BuildConfiguration::Development;
#elif !defined(DEBUG)
		buildConfig = BuildConfiguration::Shipping;
#endif

		environmentVars = getEnvironmentVariables();
		system.platform = getPlatform();
		system.architecture = getArchitecture();
		system.cpuCores = getCoreCount();
		system.osVersion = getOSVersion();
		system.cpuBrand = getCPUBrand();
		system.totalMemoryMB = getTotalMemoryMB();
		system.availableMemoryMB = getAvailableMemoryMB();

		paths.executablePath = getExecutablePath();
		paths.executableDir = getDirectory(paths.executablePath);
		paths.workingDirectory = std::filesystem::current_path();
		paths.tempPath = getTempDirectory();

		auto info = core::Application::main->getAppInfo();
		paths.userDataPath = getUserDataPath(info.organization, info.appId);
		paths.logPath = getLogPath(info.organization, info.appId);
		paths.cachePath = getCachePath(info.organization, info.appId);

		std::filesystem::create_directories(paths.userDataPath);
		std::filesystem::create_directories(paths.logPath);
		std::filesystem::create_directories(paths.cachePath);

		utils::CrashReporter::registerHandler();

		if (std::filesystem::exists(std::filesystem::path(paths.logPath) /
									"log_prev.txt"))
		{
			std::filesystem::remove(std::filesystem::path(paths.logPath) /
									"log_prev.txt");
		}

		if (std::filesystem::exists(std::filesystem::path(paths.logPath) / "log.txt"))
		{
			std::filesystem::rename(std::filesystem::path(paths.logPath) / "log.txt",
									std::filesystem::path(paths.logPath) /
										"log_prev.txt");
		}

		std::ifstream tmpLog("tmp_log.txt");

		std::ofstream log(std::filesystem::path(paths.logPath) / "log.txt",
						  std::ios_base::app);
		log << tmpLog.rdbuf();
		log.close();

		std::filesystem::remove("tmp_log.txt");

		log_remove_callback(0);

		log_add_fp(fopen((std::filesystem::path(paths.logPath) / "log.txt").c_str(), "a"),
				   LOG_TRACE);
	}

	auto getPlatform() -> std::string
	{
#if defined(_WIN32)
		return "Windows";
#elif defined(__APPLE__)
#	include <TargetConditionals.h>
#	if TARGET_OS_IPHONE
		return "iOS";
#	else
		return "macOS";
#	endif
#elif defined(__ANDROID__)
		return "Android";
#elif defined(__linux__)
		return "Linux";
#else
		return "Unknown";
#endif
	}

	auto getDirectory(const std::string& path) -> std::string
	{
		return path.substr(0, path.find_last_of("/\\"));
	}

	auto getArchitecture() -> std::string
	{
#if defined(__x86_64__) || defined(_M_X64)
		return "x64";
#elif defined(__i386__) || defined(_M_IX86)
		return "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
		return "ARM64";
#elif defined(__arm__) || defined(_M_ARM)
		return "ARM";
#else
		return "Unknown";
#endif
	}

	auto getTempDirectory() -> std::string
	{
#if defined(_WIN32)
		char buffer[MAX_PATH];
		GetTempPathA(MAX_PATH, buffer);
		return std::string(buffer);
#else
		const char* tmp = getenv("TMPDIR");
		return tmp ? std::string(tmp) : "/tmp";
#endif
	}

#if defined(_WIN32)
	auto getEnvironmentVariables() -> std::unordered_map<std::string, std::string>
	{
		std::unordered_map<std::string, std::string> envMap;
		LPCH envStrings = GetEnvironmentStringsA();
		if (!envStrings)
			return envMap;

		LPCH current = envStrings;
		while (*current)
		{
			std::string entry(current);
			size_t pos = entry.find('=');
			if (pos != std::string::npos)
			{
				std::string key = entry.substr(0, pos);
				std::string value = entry.substr(pos + 1);
				envMap[key] = value;
			}
			current += entry.size() + 1;
		}

		FreeEnvironmentStringsA(envStrings);
		return envMap;
	}

#else // POSIX (Linux/macOS)
	auto getEnvironmentVariables() -> std::unordered_map<std::string, std::string>
	{
		std::unordered_map<std::string, std::string> envMap;
		for (char** current = environ; *current; ++current)
		{
			std::string entry(*current);
			size_t pos = entry.find('=');
			if (pos != std::string::npos)
			{
				std::string key = entry.substr(0, pos);
				std::string value = entry.substr(pos + 1);
				envMap[key] = value;
			}
		}
		return envMap;
	}
#endif

#if defined(_WIN32)
	std::string getOSVersion()
	{
		OSVERSIONINFOEXW info = {};
		info.dwOSVersionInfoSize = sizeof(info);
#	pragma warning(suppress : 4996) // use GetVersionEx for simplicity
		if (GetVersionExW((LPOSVERSIONINFOW)&info))
		{
			std::wstringstream ss;
			ss << L"Windows " << info.dwMajorVersion << L"." << info.dwMinorVersion;
			return std::string(ss.str().begin(), ss.str().end());
		}
		return "Windows (Unknown)";
	}

	std::string getCPUBrand()
	{
		int info[4] = {-1};
		char brand[0x40] = {};

		__cpuid(info, 0x80000000);
		unsigned int nExIds = info[0];
		if (nExIds >= 0x80000004)
		{
			__cpuid((int*)info, 0x80000002);
			memcpy(brand, info, sizeof(info));
			__cpuid((int*)info, 0x80000003);
			memcpy(brand + 16, info, sizeof(info));
			__cpuid((int*)info, 0x80000004);
			memcpy(brand + 32, info, sizeof(info));
		}

		return std::string(brand);
	}

	uint64_t getTotalMemoryMB()
	{
		MEMORYSTATUSEX mem = {sizeof(mem)};
		GlobalMemoryStatusEx(&mem);
		return mem.ullTotalPhys / (1024 * 1024);
	}

	uint64_t getAvailableMemoryMB()
	{
		MEMORYSTATUSEX mem = {sizeof(mem)};
		GlobalMemoryStatusEx(&mem);
		return mem.ullAvailPhys / (1024 * 1024);
	}

	std::string getExecutablePath()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(nullptr, buffer, MAX_PATH);
		return std::string(buffer);
	}
#elif defined(__APPLE__)
	std::string getOSVersion()
	{
		struct utsname uts;
		if (uname(&uts) == 0)
		{
			return std::string(uts.sysname) + " " + uts.release;
		}
		return "macOS (Unknown)";
	}

	std::string getCPUBrand()
	{
		char buffer[256];
		size_t size = sizeof(buffer);
		sysctlbyname("machdep.cpu.brand_string", &buffer, &size, nullptr, 0);
		return std::string(buffer);
	}

	uint64_t getTotalMemoryMB()
	{
		int64_t mem = 0;
		size_t size = sizeof(mem);
		sysctlbyname("hw.memsize", &mem, &size, nullptr, 0);
		return mem / (1024 * 1024);
	}

	auto getAvailableMemoryMB() -> uint64_t
	{
		return getTotalMemoryMB();
	}

	std::string getExecutablePath()
	{
		char buffer[1024];
		uint32_t size = sizeof(buffer);
		if (_NSGetExecutablePath(buffer, &size) == 0)
			return std::string(buffer);
		return "";
	}

#elif defined(__linux)
	auto getOSVersion() -> std::string
	{
		std::ifstream file("/etc/os-release");
		std::string line;
		while (std::getline(file, line))
		{
			if (line.rfind("PRETTY_NAME=") == 0)
			{
				auto version = line.substr(13, line.length() - 14); // remove quotes
				return version;
			}
		}
		return "Linux (Unknown)";
	}

	auto getCPUBrand() -> std::string
	{
		std::ifstream cpuinfo("/proc/cpuinfo");
		std::string line;
		while (std::getline(cpuinfo, line))
		{
			if (line.rfind("model name") == 0)
			{
				auto pos = line.find(':');
				if (pos != std::string::npos)
				{
					return line.substr(pos + 2); // skip ": "
				}
			}
		}
		return "Unknown CPU";
	}

	auto getTotalMemoryMB() -> uint64_t
	{
		struct sysinfo info;
		if (sysinfo(&info) == 0)
		{
			return (info.totalram * info.mem_unit) / (static_cast<long>(1024 * 1024));
		}
		return 0;
	}

	auto getAvailableMemoryMB() -> uint64_t
	{
		struct sysinfo info;
		if (sysinfo(&info) == 0)
		{
			return (info.freeram * info.mem_unit) / (static_cast<long>(1024) * 1024);
		}
		return 0;
	}

	auto getExecutablePath() -> std::string
	{
		char buffer[1024];
		ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
		if (len != -1)
		{
			buffer[len] = '\0';
			return std::string(buffer);
		}
		return "";
	}
#endif

	auto getCoreCount() -> uint32_t
	{
		return std::thread::hardware_concurrency();
	}

	auto getEnvOrFallback(const char* env, const std::string& fallback) -> std::string
	{
		const char* val = getenv(env);
		return val ? std::string(val) : fallback;
	}

	auto getUserDataPath(const std::string& org, const std::string& appName)
		-> std::string
	{
#if defined(_WIN32)
		return getEnvOrFallback("APPDATA", ".") + "\\" + appName;
#else
		return getEnvOrFallback("XDG_DATA_HOME",
								getEnvOrFallback("HOME", ".") + "/.local/share") +
			   "/" + org + "/" + appName;
#endif
	}

	auto getCachePath(const std::string& org, const std::string& appName) -> std::string
	{
#if defined(_WIN32)
		return getEnvOrFallback("LOCALAPPDATA", ".") + "\\" + appName + "\\Cache";
#else
		return getEnvOrFallback("XDG_CACHE_HOME",
								getEnvOrFallback("HOME", ".") + "/.cache") +
			   "/" + org + "/" + appName;
#endif
	}

	auto getLogPath(const std::string& org, const std::string& appName) -> std::string
	{
#if DEBUG
		return getDirectory(getExecutablePath()) + "/logs";
#else
		return getUserDataPath(org, appName) + "/logs";
#endif
	}

	auto signalToString(int signal) -> std::string
	{
#ifdef __unix
		switch (signal)
		{
		case SIGSEGV:
			return "segmentation fault";

		case SIGABRT:
			return "abnormal termination";

		case SIGILL:
			return "illegal instruction";

		case SIGFPE:
			return "floating point error";

		default:
			return "unknown signal";
		}
#else
		switch (signal)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			return "access violation";

		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return "array bounds exceeded";

		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return "misalignment on datatype";

		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return "denormal operand";

		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return "you...divided by zero?";

		case EXCEPTION_FLT_INEXACT_RESULT:
			return "inexact result";

		case EXCEPTION_FLT_INVALID_OPERATION:
			return "invalid floating operation";

		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return "illegal instruction";

		case EXCEPTION_IN_PAGE_ERROR:
			return "in-page error";

		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return "you...divided by zero?";

		case EXCEPTION_PRIV_INSTRUCTION:
			return "private instruction called";

		case EXCEPTION_STACK_OVERFLOW:
			return "stack overflow, refer to stack overflow for more details";

		default:
			return "unknown exception";
		}
#endif
	}
}