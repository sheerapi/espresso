#pragma once
#include "SDL_gamecontroller.h"
#include "SDL_mouse.h"
#include "imgui_internal.h"
#include "platform/Window.h"
#include <memory>

namespace editor::internals
{
	class EditorContext
	{
	public:
		EditorContext();
		~EditorContext();

		void beginFrame();
		void endFrame();
		void processInput();

	protected:
		static auto getClipboardText(ImGuiContext* ctx) -> const char*;
		static void setClipboard(ImGuiContext* ctx, const char* text);
		static auto openInShell(ImGuiContext* ctx, const char* path) -> bool;
		static void setImeData(ImGuiContext* ctx, ImGuiViewport* viewport,
							   ImGuiPlatformImeData* data);

		void setupCursors();
        void updateMonitors();

        auto getContentScaleForDisplay(int index) -> float;

	private:
		std::unique_ptr<ImGuiContext> _context;
		platform::Window* _window;

		Uint32 _windowID;
		Uint64 _time;
		char* _clipboardTextData;
		Uint32 _ouseWindowID;
		int _mouseButtonsDown;
		SDL_Cursor* _mouseCursors[ImGuiMouseCursor_COUNT];
		SDL_Cursor* _mouseLastCursor;
		int _mouseLastLeaveFrame;
		bool _mouseCanUseGlobalState;
		bool _mouseCanUseCapture;
        bool _wantUpdateMonitors;
	};
}