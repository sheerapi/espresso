#pragma once
#include "utils/Demangle.h"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace core::internals
{
	inline unsigned long globalAssetCount;
}

namespace core
{
	class Asset
	{
	public:
		virtual ~Asset() = default;
		Asset() : _id(++core::internals::globalAssetCount) {};

		auto getName() -> std::string
		{
			return name;
		}

		[[nodiscard]] auto getID() const -> unsigned long
		{
			return _id;
		}

	protected:
		std::string name;

	private:
		unsigned long _id;
	};

	class AssetProcessor
	{
	public:
		virtual ~AssetProcessor() = default;
		virtual auto load(char* data, unsigned long size) -> std::shared_ptr<Asset> = 0;

		virtual auto getDefaultAsset() -> std::shared_ptr<Asset>
		{
			return {};
		}

		virtual auto canLoad(const std::string& extension) -> bool = 0;
		virtual auto deferredLoad() -> bool
		{
			return true;
		}
	};

	class AssetManager
	{
	public:
		static void init();
		static void shutdown();

		// index folder contents
		static void loadFolder(const std::string& path);

		// loads a bundle file and indexes its contents
		static auto loadBundle(const std::string& bundlePath) -> bool;

		// retrieves an asset by its relative path
		static auto getAsset(const std::string& path) -> std::shared_ptr<Asset>;
		static auto assetExists(const std::string& path) -> bool;

		// Clears all loaded bundles
		static void clear();

		template <typename T> static void registerProcessor()
		{
			std::lock_guard<std::mutex> lock(mutex);
			typeCheck<AssetProcessor, T>();
			processors.push_back(new T());
		}

		static std::unordered_map<std::string, std::shared_ptr<Asset>> getLoadedAssets()
		{
			return loadedAssets;
		}

	private:
		struct AssetEntry
		{
		public:
			char* path;				   // Relative asset path
			uint64_t hash;			   // Content hash
			uint64_t type;			   // File type (determined via extension)
			uint64_t offset;		   // Position in bundle file
			uint64_t uncompressedSize; // Original size
			uint64_t compressedSize;   // Size in bundle
			uint8_t compressionType;   // 0 = none, 1 = zstd
			std::string bundlePath;	   // Source bundle file
		};

		static auto readAssetData(const AssetEntry& entry) -> std::shared_ptr<Asset>;
		static auto readExternalData(const AssetEntry& entry) -> std::shared_ptr<Asset>;

		inline static std::unordered_map<std::string, AssetEntry> assetIndex;
		inline static std::unordered_map<std::string, std::shared_ptr<Asset>>
			loadedAssets;
		inline static std::vector<AssetProcessor*> processors;
		inline static std::mutex mutex;
	};
}