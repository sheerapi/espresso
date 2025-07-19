#include "editor/core/EditorContext.h"
#include "SDL_version.h"
#include "core/Application.h"
#include "imgui.h"

namespace editor::internals
{
    EditorContext::EditorContext()
    {
		IMGUI_CHECKVERSION();

		_window = core::Application::main->getWindow();

        SDL_version version;
        SDL_GetVersion(&version);

        char backendName[48];

        auto& io = ImGui::GetIO();
        io.BackendPlatformUserData = (void*)this;
		snprintf(backendName, sizeof(backendName),
				 "Espresso + SDL (%u.%u.%u)", version.major,
				 version.minor, version.patch);
            
        io.BackendPlatformUserData = backendName;
        io.BackendRendererName = "Espresso";
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        auto platformIo = ImGui::GetPlatformIO();
	}
}