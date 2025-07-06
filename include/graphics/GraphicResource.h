#pragma once

namespace graphics
{
	inline static unsigned int globalGraphicResources;

	class GraphicResource
	{
	public:
		GraphicResource() : _id(globalGraphicResources++) {};
		virtual ~GraphicResource() = default;

		[[nodiscard]] auto getId() const -> unsigned int
		{
			return _id;
		}

	private:
		unsigned int _id;
	};
}