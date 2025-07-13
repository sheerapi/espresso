#pragma once

namespace graphics
{
	class GraphicDevice
	{
	public:
		virtual ~GraphicDevice() = default;

		virtual void init() {};
	};
}