#include "core/EventManager.h"
#include "core/log.h"
#include "utils/StringUtils.h"

namespace core
{
	std::unordered_map<std::string, std::vector<EventListener>> EventManager::listeners;
	std::mutex EventManager::eventMutex;

	void EventManager::triggerEvent(const std::string& name, void* data)
	{
		if (listeners.find(stringToLower(name)) != listeners.end())
		{
			std::lock_guard<std::mutex> lock(eventMutex);

			for (auto& callback : listeners[stringToLower(name)])
			{
				callback(data);
			}
		}
	}

	void EventManager::addListener(const std::string& name, const EventListener& callback)
	{
		std::lock_guard<std::mutex> lock(eventMutex);
		listeners[stringToLower(name)].push_back(callback);
	}

	void EventManager::removeListener(const std::string& name)
	{
		if (listeners.find(stringToLower(name)) != listeners.end())
		{
			std::lock_guard<std::mutex> lock(eventMutex);
			listeners[stringToLower(name)].clear();
		}
		else
		{
			log_error("event %s not registered!", name.c_str());
		}
	}

	void EventManager::removeAll()
	{
		std::lock_guard<std::mutex> lock(eventMutex);
		listeners.clear();
	}
}