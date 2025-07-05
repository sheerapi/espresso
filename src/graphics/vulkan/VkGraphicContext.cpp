#include "graphics/vulkan/VkGraphicContext.h"
#include "SDL2/SDL_vulkan.h"
#include "core/Application.h"
#include "core/log.h"
#include "glad/volk.h"

#define es_vkMsgSeverity VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT

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
		volkLoadInstance(instance);

		_queryInstanceExtensions();
		_setupDebugCallback();

		log_info("initialized vulkan instance (%u supported extensions)",
				 extensions.size());
	}

	VkGraphicContext::~VkGraphicContext()
	{
		if (instance == nullptr)
		{
			return;
		}

#ifdef DEBUG
		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
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

#ifdef DEBUG
		requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
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

#ifdef DEBUG
		const std::vector<const char*> ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
		if (supportsValidationLayers(ValidationLayers))
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
			createInfo.ppEnabledLayerNames = ValidationLayers.data();
		}
		else
		{
			log_warn("validation layers not supported");
		}

#endif

		es_vkCall(vkCreateInstance(&createInfo, nullptr, &instance));
	}

	void VkGraphicContext::_queryInstanceExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		extensions = std::vector<VkExtensionProperties>(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
											   extensions.data());
	}

	auto VkGraphicContext::supportsValidationLayers(
		const std::vector<const char*>& layers) -> bool
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : layers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		log_trace("enabled %d validation layers out of %d", layers.size(), layerCount);

		return true;
	}

	auto VkGraphicContext::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		-> VkBool32
	{
		if (messageSeverity < es_vkMsgSeverity)
		{
			return 1U;
		}

		std::string type;
		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			type = "general";
			break;

		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			type = "validation";
			break;

		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			type = "perf";
			break;

		default:
			type = "unknown";
			break;
		}

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			log_trace("(%s) %s (%d related objects)", type.c_str(),
					  pCallbackData->pMessage, pCallbackData->objectCount);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			log_info("(%s) %s (%d related objects)", type.c_str(),
					 pCallbackData->pMessage, pCallbackData->objectCount);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			log_warn("(%s) %s (%d related objects)", type.c_str(),
					 pCallbackData->pMessage, pCallbackData->objectCount);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			log_error("(%s) %s (%d related objects)", type.c_str(),
					  pCallbackData->pMessage, pCallbackData->objectCount);
			break;

		default:
			break;
		};

		return 1U;
	}

	void VkGraphicContext::_setupDebugCallback()
	{
#ifdef DEBUG
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional

		vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);
#endif
	}
}