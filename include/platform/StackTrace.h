#pragma once
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#define UNW_LOCAL_ONLY

#if defined(_WIN32)
#	include <dbghelp.h>
#	include <psapi.h>
#	include <tlhelp32.h>
#	include <windows.h>
#	pragma comment(lib, "dbghelp.lib")
#else
#	include <dirent.h>
#	include <dwarf.h>
#	include <elfutils/libdw.h>
#	include <elfutils/libdwfl.h>
#	include <libunwind-ptrace.h>
#	include <libunwind.h>
#	include <link.h>
#	include <pthread.h>
#	include <sys/ptrace.h>
#	include <sys/syscall.h>
#	include <sys/wait.h>
#	include <unistd.h>
#	include <cxxabi.h>
#endif

namespace platform
{
	struct ThreadInfo
	{
		std::thread::id id;
		std::thread::native_handle_type handle;
		std::string name;
	};

	class StackTrace
	{
	public:
		struct Frame
		{
			void* address;
			std::string function;
			std::string module;
			std::string filename;
			int line;
		};

		static auto captureCurrentThread(std::vector<Frame>& frames, int skip = 0)
			-> bool;

		static auto captureAllThreads(
			std::unordered_map<std::thread::id, std::vector<Frame>>& allTraces) -> bool;

		static auto captureThread(std::thread::native_handle_type handle,
								  std::vector<Frame>& frames, int skip = 0) -> bool;

		static auto format(const std::vector<Frame>& frames) -> std::string;

	private:
#if defined(_WIN32)
		static bool captureThreadWindows(HANDLE thread, std::vector<Frame>& frames,
										 int skip);

		static void resolveAddress(Frame& frame);

#else // UNIX implementation

		static auto unwind(unw_cursor_t& cursor, std::vector<Frame>& frames) -> bool;

		static void resolveAddress(Frame& frame);

		static auto captureThreadLinux(std::thread::native_handle_type handle,
									   std::vector<Frame>& frames, int skip) -> bool;

		static void resolveSourceLine(Frame& frame);
#endif

		static auto getThreads() -> std::vector<ThreadInfo>;

#if defined(_WIN32)
		static std::vector<ThreadInfo> gettWindowsThreads(std::vector<ThreadInfo>& base);
		static std::string getWindowsThreadName(HANDLE hThread);
#else
		static auto getUnixThreads(std::vector<ThreadInfo>& base)
			-> std::vector<ThreadInfo>;
		static auto pthreadFromTid(pid_t tid, pthread_t& pt) -> bool;
		static auto getUnixThreadName(pid_t tid) -> std::string;

		inline static Dwfl_Callbacks dwflCallbacks = {
			.find_elf = dwfl_linux_proc_find_elf,
			.find_debuginfo = dwfl_standard_find_debuginfo,
			.section_address = dwfl_offline_section_address,
		};
#endif

		static auto getThreadName(std::thread::native_handle_type handle) -> std::string
		{
#if defined(_WIN32)
			return getWindowsThreadName(reinterpret_cast<HANDLE>(handle));
#else
			char name[16] = {0};
			if (pthread_getname_np(handle, name, sizeof(name)) != 0)
			{
				return "Unknown";
			}
			return name;
#endif
		}
	};
}