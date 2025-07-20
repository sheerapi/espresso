#include "input/Input.h"
#include "SDL_clipboard.h"
#include "SDL_misc.h"

namespace core
{
	void Input::setClipboard(const std::string& string)
    {
        SDL_SetClipboardText(string.c_str());
    }

	auto Input::getClipboard() -> std::string
    {
        return {SDL_GetClipboardText()};
    }

	void Input::openUrl(const std::string& url)
    {
        SDL_OpenURL(url.c_str());
    }
}