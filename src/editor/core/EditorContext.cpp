#include "editor/core/EditorContext.h"
#include "SDL_syswm.h"
#include "SDL_version.h"
#include "core/Application.h"
#include "core/log.h"
#include "imgui.h"

namespace editor::internals
{
	EditorContext::EditorContext()
	{
		IMGUI_CHECKVERSION();

		_window = core::Application::main->getWindow();

		_windowID = SDL_GetWindowID((SDL_Window*)_window->getWindowHandle());

		_context = std::unique_ptr<ImGuiContext>(ImGui::CreateContext());
		ImGui::SetCurrentContext(_context.get());

		SDL_version version;
		SDL_GetVersion(&version);

		char backendName[48];

		auto& io = ImGui::GetIO();
		io.BackendPlatformUserData = (void*)this;
		snprintf(backendName, sizeof(backendName), "Espresso + SDL (%u.%u.%u)",
				 version.major, version.minor, version.patch);

		io.BackendPlatformUserData = backendName;
		io.BackendRendererName = "Espresso";
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		auto platformIo = ImGui::GetPlatformIO();

		platformIo.Platform_SetClipboardTextFn = setClipboard;
		platformIo.Platform_GetClipboardTextFn = getClipboardText;
		platformIo.Platform_OpenInShellFn = openInShell;
		platformIo.Platform_SetImeDataFn = setImeData;

		setupCursors();

		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		main_viewport->PlatformHandle = (void*)(intptr_t)_windowID;
		main_viewport->PlatformHandleRaw = nullptr;

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if (SDL_GetWindowWMInfo((SDL_Window*)_window->getWindowHandle(), &info) != 0U)
		{
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
			main_viewport->PlatformHandleRaw = (void*)info.info.win.window;
#elif defined(__APPLE__) && defined(SDL_VIDEO_DRIVER_COCOA)
			main_viewport->PlatformHandleRaw = (void*)info.info.cocoa.window;
#endif
		}

        updateMonitors();

		log_info("created editor context");
	}

	void EditorContext::updateMonitors()
	{
		ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
		platform_io.Monitors.resize(0);
		_wantUpdateMonitors = false;
		int display_count = SDL_GetNumVideoDisplays();
		for (int n = 0; n < display_count; n++)
		{
			// Warning: the validity of monitor DPI information on Windows depends on the
			// application DPI awareness settings, which generally needs to be set in the
			// manifest or at runtime.
			ImGuiPlatformMonitor monitor;
			SDL_Rect r;
			SDL_GetDisplayBounds(n, &r);
			monitor.MainPos = monitor.WorkPos = ImVec2((float)r.x, (float)r.y);
			monitor.MainSize = monitor.WorkSize = ImVec2((float)r.w, (float)r.h);
#if SDL_HAS_USABLE_DISPLAY_BOUNDS
			if (SDL_GetDisplayUsableBounds(n, &r) == 0 && r.w > 0 && r.h > 0)
			{
				monitor.WorkPos = ImVec2((float)r.x, (float)r.y);
				monitor.WorkSize = ImVec2((float)r.w, (float)r.h);
			}
#endif
			float dpi_scale = getContentScaleForDisplay(n);
			if (dpi_scale <= 0.0F)
			{
				continue; // Some accessibility applications are declaring virtual
			}
			// monitors with a DPI of 0, see #7902.
			monitor.DpiScale = dpi_scale;
			monitor.PlatformHandle = (void*)(intptr_t)n;
			platform_io.Monitors.push_back(monitor);
		}
	}

	void EditorContext::setupCursors()
	{
		_mouseCursors[ImGuiMouseCursor_Arrow] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		_mouseCursors[ImGuiMouseCursor_TextInput] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		_mouseCursors[ImGuiMouseCursor_ResizeAll] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		_mouseCursors[ImGuiMouseCursor_ResizeNS] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
		_mouseCursors[ImGuiMouseCursor_ResizeEW] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
		_mouseCursors[ImGuiMouseCursor_ResizeNESW] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
		_mouseCursors[ImGuiMouseCursor_ResizeNWSE] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
		_mouseCursors[ImGuiMouseCursor_Hand] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		/*_mouseCursors[ImGuiMouseCursor_Wait] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
		_mouseCursors[ImGuiMouseCursor_Progress] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);*/ // apparently these arent found
		_mouseCursors[ImGuiMouseCursor_NotAllowed] =
			SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
	}

	EditorContext::~EditorContext()
	{
	}

	auto EditorContext::getContentScaleForDisplay(int index) -> float
	{
#if SDL_HAS_PER_MONITOR_DPI
#	if !defined(__APPLE__) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
		float dpi = 0.0f;
		if (SDL_GetDisplayDPI(display_index, &dpi, nullptr, nullptr) == 0)
			return dpi / 96.0f;
#	endif
#endif
		IM_UNUSED(index);
		return 1.0F;
	}
}