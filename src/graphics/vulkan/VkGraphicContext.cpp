#include "graphics/vulkan/VkGraphicContext.h"
#include "SDL2/SDL_vulkan.h"
#include "core/Application.h"
#include "core/log.h"

namespace graphics::vk
{
	void VkGraphicContext::init(platform::Window* window)
	{
		if (volkInitialize() == VK_ERROR_INITIALIZATION_FAILED)
		{
			log_fatal("failed to initialize Vulkan");
			return;
		}

		this->window = window;

		_retrieveRequiredInstanceExtensions();
		_createInstance();
		_queryInstanceExtensions();

		log_info("initialized vulkan instance (%d supported extensions)",
				 extensions.size());
	}

	VkGraphicContext::~VkGraphicContext()
	{
		vkDestroyInstance(instance, nullptr);
		volkFinalize();
	}

	void VkGraphicContext::makeCurrent()
	{
	}

	auto VkGraphicContext::getBackend() -> uint32_t
	{
		return 0x10000000; // SDL_WINDOW_VULKAN
	}

	void VkGraphicContext::setVsync(bool vsync)
	{
	}

	void VkGraphicContext::_retrieveRequiredInstanceExtensions()
	{
		uint32_t extensionCount;
		SDL_Vulkan_GetInstanceExtensions((SDL_Window*)window->getWindowHandle(),
										 &extensionCount, nullptr);
		requiredInstanceExtensions.reserve(extensionCount);

		SDL_Vulkan_GetInstanceExtensions((SDL_Window*)window->getWindowHandle(),
										 &extensionCount,
										 requiredInstanceExtensions.data());
	}

	void VkGraphicContext::_createInstance()
	{
		auto appName = core::Application::main->getName();

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = appName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Espresso";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = requiredInstanceExtensions.size();
		createInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();

		es_vkCall(vkCreateInstance(&createInfo, nullptr, &instance));
	}

	void VkGraphicContext::_queryInstanceExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		extensions.reserve(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
											   extensions.data());
	}
}