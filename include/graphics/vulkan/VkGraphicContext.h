#pragma once
#include "glad/volk.h"
#include "graphics/GraphicContext.h"
#include "graphics/vulkan/VkGraphicDevice.h"
#include <cassert>
#include <memory>
#include <vector>
#include "core/log.h"

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

		auto getSurface() -> VkSurfaceKHR
		{
			return surface;
		}

	protected:
		std::vector<const char*> requiredInstanceExtensions;
		std::vector<VkExtensionProperties> extensions;
		platform::Window* window;

		VkInstance instance{nullptr};
		VkSurfaceKHR surface{nullptr};

		VkDebugUtilsMessengerEXT debugMessenger;

	private:
		void _createInstance();
		void _retrieveRequiredInstanceExtensions();
		void _queryInstanceExtensions();
        void _setupDebugCallback();
        void _pickDevice();
		
		static auto supportsValidationLayers(const std::vector<const char*>& layers)
			-> bool;

		static auto debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32;
	};
}