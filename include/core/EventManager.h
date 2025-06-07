#pragma once
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace core
{
	using EventListener = std::function<void(void*)>;

	class EventManager
	{
	public:
		static void triggerEvent(const std::string& name, void* data = nullptr);

		static void addListener(const std::string& name, const EventListener& callback);
		static void removeListener(const std::string& name);

		static void removeAll();

	private:
		static std::unordered_map<std::string, std::vector<EventListener>>
			listeners;
		static std::mutex eventMutex;
	};
}