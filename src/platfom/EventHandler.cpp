#include "platform/EventHandler.h"
#include "core/Application.h"
#include "core/log.h"

namespace internals
{
	void handleEvent(SDL_Event e)
    {
        switch ((SDL_EventType)e.type)
        {
		case SDL_QUIT:
		case SDL_APP_TERMINATING:
			core::Application::main->getWindow()->close();
            break;
		
		case SDL_APP_LOWMEMORY:
            log_warn("memory is running low!");
            break;

		case SDL_WINDOWEVENT:
		case SDL_SYSWMEVENT:
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEWHEEL:
		case SDL_JOYAXISMOTION:
		case SDL_JOYBALLMOTION:
		case SDL_JOYHATMOTION:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		case SDL_JOYDEVICEADDED:
		case SDL_JOYDEVICEREMOVED:
		case SDL_JOYBATTERYUPDATED:
		case SDL_CONTROLLERAXISMOTION:
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_CONTROLLERDEVICEREMAPPED:
		case SDL_CONTROLLERTOUCHPADDOWN:
		case SDL_CONTROLLERTOUCHPADMOTION:
		case SDL_CONTROLLERTOUCHPADUP:
		case SDL_CONTROLLERSENSORUPDATE:
		case SDL_CONTROLLERSTEAMHANDLEUPDATED:
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
		case SDL_FINGERMOTION:
		case SDL_DOLLARGESTURE:
		case SDL_DOLLARRECORD:
		case SDL_MULTIGESTURE:
		case SDL_CLIPBOARDUPDATE:
		case SDL_DROPFILE:
		case SDL_DROPTEXT:
		case SDL_DROPBEGIN:
		case SDL_DROPCOMPLETE:
		default:
			break;
		}
	}
}