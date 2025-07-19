#pragma once
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

    private:
        std::unique_ptr<ImGuiContext> _context;
        platform::Window* _window;
    };
}