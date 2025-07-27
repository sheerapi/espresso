#include "platform/StackTrace.h"
#include <algorithm>
#include <elfutils/libdwfl.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

namespace platform
{
	auto StackTrace::captureCurrentThread(std::vector<Frame>& frames, int skip) -> bool
	{
		frames.clear();
#if defined(_WIN32)
		return captureThreadWindows(GetCurrentThread(), frames, skip);
#else
		unw_cursor_t cursor;
		unw_context_t context;

		unw_getcontext(&context);
		unw_init_local(&cursor, &context);

		// Skip current function frames
		while (skip-- > 0 && unw_step(&cursor) > 0)
		{
			;
		}

		return unwind(cursor, frames);
#endif
	}

	auto StackTrace::captureAllThreads(
		std::unordered_map<std::thread::id, std::vector<Frame>>& allTraces) -> bool
	{
		allTraces.clear();
		std::mutex map_mutex;
		bool success = true;

		auto thread_capture =
			[&](std::thread::native_handle_type handle, std::thread::id id)
		{
			std::vector<Frame> frames;
			bool result = captureThread(handle, frames, 3);

			std::lock_guard<std::mutex> lock(map_mutex);
			if (result)
			{
				allTraces.emplace(id, std::move(frames));
			}
			else
			{
				success = false;
			}
		};

		// Capture current thread first
		std::vector<Frame> main_frames;
		if (captureCurrentThread(main_frames, 4))
		{
			allTraces.emplace(std::this_thread::get_id(), std::move(main_frames));
		}

		auto threads = getThreads();
		
		// Capture other threads
		for (auto& t : threads)
		{
			if (t.id != std::this_thread::get_id())
			{
				thread_capture(t.handle, t.id);
			}
		}

		return success;
	}

	auto StackTrace::captureThread(std::thread::native_handle_type handle,
								   std::vector<Frame>& frames, int skip) -> bool
	{
		frames.clear();
#if defined(_WIN32)
		return captureThreadWindows(handle, frames, skip);
#else
		return captureThreadLinux(handle, frames, skip);
#endif
	}

	auto StackTrace::format(const std::vector<Frame>& frames) -> std::string
	{
		std::ostringstream ss;
		for (size_t i = 0; i < frames.size(); ++i)
		{
			ss << "#" << i << " " << frames[i].address << " in " << frames[i].function;

			if (!frames[i].filename.empty())
			{
				ss << " (" << frames[i].filename;
				if (frames[i].line > 0)
				{
					ss << ":" << frames[i].line;
				}
				ss << ")";
			}

			if (!frames[i].module.empty())
			{
				ss << " [" << frames[i].module << "]";
			}

			ss << "\n";
		}
		return ss.str();
	}

#ifdef _WIN32
	bool StackTrace::captureThreadWindows(HANDLE thread, std::vector<Frame>& frames,
										  int skip)
	{
		CONTEXT context = {0};
		context.ContextFlags = CONTEXT_FULL;

		if (SuspendThread(thread) == (DWORD)-1)
			return false;

		bool success = GetThreadContext(thread, &context);
		ResumeThread(thread);

		if (!success)
			return false;

		STACKFRAME64 frame = {0};
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Mode = AddrModeFlat;

#	ifdef _M_IX86
		frame.AddrPC.Offset = context.Eip;
		frame.AddrFrame.Offset = context.Ebp;
		frame.AddrStack.Offset = context.Esp;
		DWORD machine = IMAGE_FILE_MACHINE_I386;
#	elif _M_X64
		frame.AddrPC.Offset = context.Rip;
		frame.AddrFrame.Offset = context.Rbp;
		frame.AddrStack.Offset = context.Rsp;
		DWORD machine = IMAGE_FILE_MACHINE_AMD64;
#	else
#		error "Unsupported architecture"
#	endif

		HANDLE process = GetCurrentProcess();
		SymRefreshModuleList(process);

		while (StackWalk64(machine, process, thread, &frame, &context, nullptr,
						   SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
		{
			if (frame.AddrPC.Offset == 0)
				break;
			if (skip-- > 0)
				continue;

			Frame f;
			f.address = (void*)frame.AddrPC.Offset;
			resolveAddress(f);
			frames.push_back(std::move(f));
		}

		return true;
	}

	void StackTrace::resolveAddress(Frame& frame)
	{
		static std::once_flag init_flag;
		std::call_once(init_flag,
					   []
					   {
						   SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME |
										 SYMOPT_DEFERRED_LOADS);
						   SymInitialize(GetCurrentProcess(), nullptr, TRUE);
					   });

		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		DWORD64 displacement = 0;
		if (SymFromAddr(GetCurrentProcess(), (DWORD64)frame.address, &displacement,
						symbol))
		{
			frame.function = symbol->Name;
		}

		IMAGEHLP_LINE64 line = {sizeof(IMAGEHLP_LINE64)};
		DWORD line_displacement;
		if (SymGetLineFromAddr64(GetCurrentProcess(), (DWORD64)frame.address,
								 &line_displacement, &line))
		{
			frame.filename = line.FileName;
			frame.line = line.LineNumber;
		}

		HMODULE module_handle;
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
							  (LPCTSTR)frame.address, &module_handle))
		{
			char modname[MAX_PATH];
			GetModuleFileNameA(module_handle, modname, MAX_PATH);
			frame.module = modname;
		}
	}
#else
	auto StackTrace::unwind(unw_cursor_t& cursor, std::vector<Frame>& frames) -> bool
	{
		while (unw_step(&cursor) > 0)
		{
			Frame f;
			unw_word_t ip;
			unw_get_reg(&cursor, UNW_REG_IP, &ip);
			if (ip == 0)
			{
				break;
			}

			f.address = reinterpret_cast<void*>(ip);
			resolveAddress(f);
			frames.push_back(std::move(f));
		}
		return !frames.empty();
	}

	void StackTrace::resolveAddress(Frame& frame)
	{
		// Function name resolution
		Dl_info dl_info;
		if ((dladdr(frame.address, &dl_info) != 0) && (dl_info.dli_sname != nullptr))
		{
			int status = -4;
			char* demangled =
				abi::__cxa_demangle(dl_info.dli_sname, nullptr, nullptr, &status);
			frame.function = (status == 0) ? demangled : dl_info.dli_sname;
			free(demangled);

			if (dl_info.dli_fname != nullptr)
			{
				frame.module = dl_info.dli_fname;
			}
		}

		// Source line resolution (requires libdw)
		resolveSourceLine(frame);
	}

	auto StackTrace::captureThreadLinux(std::thread::native_handle_type handle,
										std::vector<Frame>& frames, int skip) -> bool
	{
		auto tid = static_cast<pid_t>(handle);
		int status;
		bool success = false;
		pid_t child = fork();

		if (child == 0)
		{ // Child process
			if (ptrace(PTRACE_ATTACH, tid, nullptr, nullptr) == 0)
			{
				waitpid(tid, &status, __WALL);
				if (WIFSTOPPED(status))
				{
					unw_addr_space_t as = unw_create_addr_space(&_UPT_accessors, 0);
					void* context = _UPT_create(tid);
					unw_cursor_t cursor;

					if (unw_init_remote(&cursor, as, context) == 0)
					{
						// Skip requested frames
						while (skip-- > 0 && unw_step(&cursor) > 0)
						{
							;
						}

						unwind(cursor, frames);
						success = true;
					}
					_UPT_destroy(context);
					unw_destroy_addr_space(as);
				}
				ptrace(PTRACE_DETACH, tid, nullptr, nullptr);
			}
			_exit(0); // Child exits
		}
		else if (child > 0)
		{ // Parent
			waitpid(child, &status, 0);
			success = (WIFEXITED(status) && WEXITSTATUS(status) == 0);
		}

		return success;
	}

	void StackTrace::resolveSourceLine(Frame& frame)
	{
		// Now use libdw for detailed source information
		static std::once_flag dwfl_init_flag;
		static Dwfl* dwfl = nullptr;
		static std::mutex dwfl_mutex;

		std::call_once(dwfl_init_flag,
					   []
					   {
						   // Initialize DWARF context
						   dwfl = dwfl_begin(&dwflCallbacks);
						   if (dwfl)
						   {
							   // Report current process
							   if (dwfl_linux_proc_report(dwfl, getpid()))
							   {
								   dwfl_end(dwfl);
								   dwfl = nullptr;
							   }
							   dwfl_report_end(dwfl, nullptr, nullptr);
						   }
					   });

		if (dwfl == nullptr)
		{
			return;
		}

		std::lock_guard<std::mutex> lock(dwfl_mutex);
		auto addr = reinterpret_cast<Dwarf_Addr>(frame.address);

		// Find the module containing the address
		Dwfl_Module* module = dwfl_addrmodule(dwfl, addr);
		if (module == nullptr)
		{
			return;
		}

		frame.module = dwfl_module_info(module, nullptr, nullptr, nullptr, nullptr,
										nullptr, nullptr, nullptr);

		// Get function name from DWARF if not already set
		if (frame.function.empty())
		{
			const char* symname = dwfl_module_addrname(module, addr);
			if (symname != nullptr)
			{
				int status = -4;
				char* demangled = abi::__cxa_demangle(symname, nullptr, nullptr, &status);
				frame.function = (status == 0) ? demangled : symname;
				free(demangled);
			}
		}

		// Get source file and line information
		Dwfl_Line* line = dwfl_module_getsrc(module, addr);
		if (line != nullptr)
		{
			Dwarf_Addr lineaddr;
			const char* src_file =
				dwfl_lineinfo(line, &lineaddr, &frame.line, nullptr, nullptr, nullptr);
			if (src_file != nullptr)
			{
				frame.filename = src_file;
			}
		}
	}
#endif

	auto StackTrace::getThreads() -> std::vector<ThreadInfo>
	{
		std::vector<ThreadInfo> threads;

#ifdef _WIN32
		threads.push_back(
			{std::this_thread::get_id(),
			 reinterpret_cast<std::thread::native_handle_type>(GetCurrentThread()),
			 GetThreadName(GetCurrentThread())});

		return getWindowsThreads(threads);
#else
		return getUnixThreads(threads);
#endif
	}

#ifdef _WIN32
	std::vector<ThreadInfo> StackTrace::getWindowsThreads(std::vector<ThreadInfo>& base)
	{
		std::vector<ThreadInfo> threads = base;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
		{
			return threads;
		}

		THREADENTRY32 te;
		te.dwSize = sizeof(THREADENTRY32);

		if (Thread32First(snapshot, &te))
		{
			DWORD pid = GetCurrentProcessId();
			do
			{
				if (te.th32OwnerProcessID == pid &&
					te.th32ThreadID != GetCurrentThreadId())
				{
					HANDLE hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION |
													THREAD_SUSPEND_RESUME,
												FALSE, te.th32ThreadID);

					if (hThread)
					{
						threads.push_back(
							{std::thread::id(
								 reinterpret_cast<std::thread::native_handle_type>(
									 hThread)), // Not directly mappable
							 reinterpret_cast<std::thread::native_handle_type>(hThread),
							 getWindowsThreadName(hThread)});
					}
				}
			} while (Thread32Next(snapshot, &te));
		}

		CloseHandle(snapshot);
		return threads;
	}

	std::string StackTrace::getWindowsThreadName(HANDLE hThread)
	{
		using THREAD_NAME_INFO = struct
		{
			DWORD dwType;	  // must be 0x1000
			LPCSTR szName;	  // pointer to name (in user addr space)
			DWORD dwThreadID; // thread ID (-1=caller thread)
			DWORD dwFlags;	  // reserved for future use, must be zero
		};

		THREAD_NAME_INFO info;
		info.dwType = 0x1000;
		info.szName = nullptr;
		info.dwThreadID = GetThreadId(hThread);
		info.dwFlags = 0;

		__try
		{
			RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR),
						   reinterpret_cast<ULONG_PTR*>(&info));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}

		// Try to get thread name through other methods
		wchar_t* name = nullptr;
		if (SUCCEEDED(GetThreadDescription(hThread, &name)))
		{
			std::wstring wname(name);
			LocalFree(name);
			return std::string(wname.begin(), wname.end());
		}

		return "Unknown";
	}
#else
	auto StackTrace::getUnixThreads(std::vector<ThreadInfo>& base)
		-> std::vector<ThreadInfo>
	{
		std::vector<ThreadInfo> threads = base;
		DIR* task_dir = opendir("/proc/self/task");
		if (task_dir == nullptr)
		{
			return threads;
		}

		pid_t self_tid = syscall(SYS_gettid);
		struct dirent* entry;

		while ((entry = readdir(task_dir)) != nullptr)
		{
			if (entry->d_name[0] == '.')
			{
				continue;
			}

			pid_t tid = static_cast<pid_t>(std::stoi(entry->d_name));
			if (tid == self_tid)
			{
				continue; // Skip current thread
			}

			// Convert tid to pthread_t
			pthread_t pt;
			if (pthreadFromTid(tid, pt))
			{
				threads.push_back({std::thread::id(pt), pt, getUnixThreadName(tid)});
			}
		}

		closedir(task_dir);
		return threads;
	}

	auto StackTrace::pthreadFromTid(pid_t tid, pthread_t& pt) -> bool
	{
		pt = static_cast<pthread_t>(tid);
		return true;
	}

	auto StackTrace::getUnixThreadName(pid_t tid) -> std::string
	{
		char path[256];
		snprintf(path, sizeof(path), "/proc/self/task/%d/comm", tid);

		std::ifstream comm(path);
		if (comm)
		{
			std::string name;
			std::getline(comm, name);
			return name;
		}
		return "Unknown";
	}
#endif
}