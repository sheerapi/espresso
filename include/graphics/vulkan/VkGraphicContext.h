#pragma once
#include "glad/volk.h"
#include "graphics/GraphicContext.h"
#include <vector>

#ifdef DEBUG
#	define es_vkCall(call)                                                              \
		{                                                                                \
			auto result = call;                                                          \
			if (result != VK_SUCCESS)                                                    \
			{                                                                            \
				log_error("failed to execute %s: %d", #call, result);                    \
			}                                                                            \
		}
#else
#	define es_vkCall(call) call
#endif

namespace graphics::vk
{
	class VkGraphicContext : public GraphicContext
	{
	public:
		~VkGraphicContext() override;

		void init(platform::Window* window) override;
		void makeCurrent() override;
		auto getBackend() -> uint32_t override;
		void setVsync(bool vsync) override;

	protected:
		std::vector<const char*> requiredInstanceExtensions;
		std::vector<VkExtensionProperties> extensions;
		platform::Window* window;

		VkInstance instance;

	private:
		void _createInstance();
		void _retrieveRequiredInstanceExtensions();
        void _queryInstanceExtensions();
	};
}