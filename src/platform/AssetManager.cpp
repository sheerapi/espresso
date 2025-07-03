#include "platform/AssetManager.h"
#include "core/log.h"
#include "platform/JobScheduler.h"
#include "utils/PerformanceTimer.h"
#include <filesystem>
#include <mutex>
#include <sys/stat.h>

#define es_bundleSpec 2

namespace core
{
	void AssetManager::init()
	{
		if (std::filesystem::exists("assets"))
		{
			loadFolder("assets");
		}

		if (std::filesystem::exists("../assets"))
		{
			loadFolder("../assets");
		}
	}

	void AssetManager::shutdown()
	{
		clear();

		for (auto& loader : processors)
		{
			delete loader;
		}

		processors.clear();
	}

	void AssetManager::loadFolder(const std::string& path)
	{
		es_stopwatch();

		uint64_t assetCount = 0;

		for (const auto& file : std::filesystem::recursive_directory_iterator(path))
		{
			if (file.is_regular_file() && file.path().extension() == ".bundle")
			{
				loadBundle(file.path());
				continue;
			}

			if (!file.is_regular_file())
			{
				continue;
			}

			// crreate file entry
			AssetEntry entry;
			auto entryPath =
				std::filesystem::relative(file.path(), std::filesystem::canonical(path));

			entry.path = new char[entryPath.string().size() + 1];
			strcpy(entry.path, entryPath.c_str());

			entry.hash = std::hash<std::string>{}(entry.path);
			entry.bundlePath = ":" + std::filesystem::canonical(path).string();

			struct stat fileStat;
			stat(file.path().c_str(), &fileStat);

			entry.uncompressedSize = fileStat.st_size;
			entry.compressionType = 0;
			entry.compressedSize = entry.uncompressedSize;

			assetIndex[std::string(entry.path)] = entry;
			assetCount++;
		}

		log_info("loaded bundle %s with %d assets (%d total loaded assets)", path.c_str(),
				 assetCount, assetIndex.size());
	}

	auto AssetManager::loadBundle(const std::string& bundlePath) -> bool
	{
		if (std::filesystem::is_directory(bundlePath))
		{
			for (const auto& entry :
				 std::filesystem::recursive_directory_iterator(bundlePath))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".bundle")
				{
					loadBundle(entry.path());
				}
			}

			return false;
		}

		es_stopwatch();

		std::lock_guard<std::mutex> lock(mutex);

		std::ifstream file(bundlePath, std::ios::binary);
		if (!file)
		{
			log_error("failed to open bundle %s", bundlePath.c_str());
			return false;
		}

		// Read header
		char magic[5] = {0};
		file.read(magic, 4);
		if (strcmp(magic, "BNDL") != 0)
		{
			log_error("invalid bundle %s", bundlePath.c_str());
			return false;
		}

		uint8_t version;
		file.read(reinterpret_cast<char*>(&version), 1);
		if (version != es_bundleSpec)
		{
			log_error("unsupported bundle version for %s (currently v%d, got v%d)",
					  bundlePath.c_str(), es_bundleSpec, version);
			return false;
		}

		uint64_t indexOffset;
		file.read(reinterpret_cast<char*>(&indexOffset), 8);

		uint32_t assetCount;
		file.read(reinterpret_cast<char*>(&assetCount), 4);

		// Read index table
		file.seekg((long)indexOffset);
		for (uint32_t i = 0; i < assetCount; ++i)
		{
			AssetEntry entry;
			entry.bundlePath = bundlePath;

			// Read path
			uint16_t pathLength;
			file.read(reinterpret_cast<char*>(&pathLength), 2);
			entry.path = new char[pathLength + 1];
			file.read(entry.path, pathLength);
			entry.path[pathLength] = '\0';

			// Read metadata
			file.read(reinterpret_cast<char*>(&entry.hash), 8);
			file.read(reinterpret_cast<char*>(&entry.offset), 8);
			file.read(reinterpret_cast<char*>(&entry.uncompressedSize), 8);
			file.read(reinterpret_cast<char*>(&entry.compressedSize), 8);
			file.read(reinterpret_cast<char*>(&entry.compressionType), 1);

			// Handle duplicates (last-loaded wins)
			if (auto it = assetIndex.find(entry.path); it != assetIndex.end())
			{
				log_warn("overwriting asset %s from previous bundle", entry.path);
			}
			assetIndex[entry.path] = entry;
		}

		log_trace("loaded bundle %s with %d assets (%d total loaded assets)",
				  bundlePath.c_str(), assetCount, assetIndex.size());

		return true;
	}

	auto AssetManager::getAsset(const std::string& path) -> std::shared_ptr<Asset>
	{
		auto it = assetIndex.find(path);
		if (it == assetIndex.end())
		{
			log_error("asset %s not found", path.c_str());
			return nullptr;
		}

		const AssetEntry& entry = it->second;

		if (entry.bundlePath.rfind(':', 0) == 0)
		{
			return {readExternalData(entry)};
		}

		return {readAssetData(entry)};
	}

	auto AssetManager::readAssetData(const AssetEntry& entry) -> std::shared_ptr<Asset>
	{
		std::lock_guard<std::mutex> lock(mutex);

		std::ifstream file(entry.bundlePath, std::ios::binary);
		if (!file)
		{
			log_error("failed to open bundle %s", entry.bundlePath.c_str());
			return nullptr;
		}

		file.seekg(entry.offset);
		char* buffer = new char[entry.uncompressedSize];

		// replace this with decompression code
		if (!file.read(buffer, entry.uncompressedSize))
		{
			log_error("failed to read asset %s", entry.path);
			return nullptr;
		}

		for (auto& loader : processors)
		{
			if (loader->canLoad(std::filesystem::path(entry.path).extension()))
			{
				loadedAssets[entry.path] = loader->getDefaultAsset();
				::internals::JobScheduler::submit(
					[=]() { loader->load(buffer, entry.uncompressedSize); });
				return {loadedAssets[entry.path]};
			}
		}

		return nullptr;
	}

	auto AssetManager::readExternalData(const AssetEntry& entry) -> std::shared_ptr<Asset>
	{
		std::lock_guard<std::mutex> lock(mutex);
		auto bundlePath = entry.bundlePath;

		std::ifstream file(
			(std::filesystem::path(bundlePath.substr(1)) / entry.path).string(),
			std::ios::binary);

		if (!file)
		{
			log_error("failed to open asset %s",
					  (std::filesystem::path(bundlePath.substr(1)) / entry.path).c_str());
			return nullptr;
		}

		char* buffer = new char[entry.uncompressedSize];

		if (!file.read(buffer, entry.uncompressedSize))
		{
			log_error("failed to read asset %s", entry.path);
			return nullptr;
		}

		for (auto& loader : processors)
		{
			if (loader->canLoad(std::filesystem::path(entry.path).extension()))
			{
				loadedAssets[entry.path] = loader->getDefaultAsset();
				::internals::JobScheduler::submit(
					[=]() { loader->load(buffer, entry.uncompressedSize); });
				return {loadedAssets[entry.path]};
			}
		}

		return nullptr;
	}

	auto AssetManager::assetExists(const std::string& path) -> bool
	{
		std::lock_guard<std::mutex> lock(mutex);
		return assetIndex.find(path) != assetIndex.end();
	}

	void AssetManager::clear()
	{
		std::lock_guard<std::mutex> lock(mutex);
		assetIndex.clear();
	}
}