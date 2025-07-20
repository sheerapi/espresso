#pragma once
#include "input/Scancode.h"
#include "utils/math/Vector2.h"
#include <bitset>

namespace core
{
    class Input
    {
    public:
		static auto getKey(Scancode key) -> bool;
		static auto getKeyDown(Scancode key) -> bool;
		static auto getKeyUp(Scancode key) -> bool;
		static auto getButton(MouseButton button) -> bool;
		static auto getButtonDown(MouseButton button) -> bool;
		static auto getButtonUp(MouseButton button) -> bool;
        static auto getMousePos() -> math::Vector2;

		static void setClipboard(const std::string& string);
		static auto getClipboard() -> std::string;
		static void openUrl(const std::string& url);

	private:
		inline static std::bitset<es_maxScancodes> keys;
		inline static std::bitset<es_maxScancodes> lastKeys;

		inline static std::bitset<es_maxMouseButtons> buttons;
		inline static std::bitset<es_maxMouseButtons> lastButtons;

		inline static math::Vector2 mousePos;
	};
}