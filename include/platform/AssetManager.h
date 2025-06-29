#pragma once
#include "physfs.h"
#include "utils/Demangle.h"
#include "zstd.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

namespace core
{
	inline static int globalAssetCount;

	class Asset
	{
	public:
		Asset() : _id(globalAssetCount++) {};

		virtual ~Asset();

		virtual auto load(char* data, size_t size) -> bool
		{
			return true;
		}
		virtual void unload() {};
		virtual void use()
		{
		}
		virtual void setUp()
		{
		}
		virtual void preSetUp()
		{
		}

		virtual auto loadFromMemory(char* data, size_t size) -> bool
		{
			return load(data, size);
		}

		void onLoad(const std::function<void(Asset*, bool)>& callback)
		{
			if (loaded)
			{
				callback(this, succeeded);
			}
			else
			{
				_loadCallbacks.push_back(callback);
			}
		}

		[[nodiscard]] auto getPath() const -> std::string
		{
			return _path;
		}
		[[nodiscard]] auto getId() const -> int
		{
			return _id;
		}

		[[nodiscard]] auto isLoaded() const -> bool
		{
			return loaded;
		}

		[[nodiscard]] auto getSize() const -> size_t
		{
			return _size;
		}

		[[nodiscard]] auto getAssetType() const -> std::string
		{
			return getNamespaceFreeName(demangle(typeid(*this).name()));
		}

	protected:
		bool loaded{false};
		bool succeeded{false};

	private:
		std::string _path;
		int _id;
		size_t _size;
		std::vector<std::function<void(Asset*, bool)>> _loadCallbacks;

		friend class AssetManager;
	};

	struct ManifestEntry
	{
	public:
		std::string path;
		uint64_t offset;
		uint64_t size;
		uint64_t realSize;
		int64_t createTime;
		int64_t lastAccessTime;
		int64_t lastWriteTime;
		bool real{true};
	};

	struct EapkInstanceData
	{
	public:
		PHYSFS_Io* io;
		std::unordered_map<std::string, ManifestEntry> manifest;
		std::unordered_map<std::string, std::vector<std::string>> directories;
	};

	struct ZstdFileHandle
	{
	public:
		PHYSFS_Io io;					// Must be first (base structure)
		PHYSFS_Io* arcIo;				// Underlying archive I/O handle
		ZSTD_DStream* dStream;			// Zstd decompression context
		ZSTD_inBuffer zstdInput;		// Compressed input buffer
		ZSTD_outBuffer zstdOutput;		// Decompressed output buffer
		PHYSFS_uint64 startOffset;		// Start of compressed data in archive
		PHYSFS_uint64 compressedSize;	// Size of compressed data
		PHYSFS_uint64 decompressedSize; // Size of original file
		PHYSFS_uint64 decompressedPos;	// Current read position (uncompressed)
		char* inBuf;					// Input buffer for compressed chunks
		char* outBuf;					// Output buffer for decompressed data
		size_t inBufSize;				// Size of input buffer (e.g., 128KB)
		size_t outBufSize;				// Size of output buffer (e.g., 128KB)
		size_t outBufAvailable;			// Bytes available in output buffer
	};

	class AssetManager
	{
	public:
		template <typename T>
		static auto load(const std::string& path) -> std::shared_ptr<T>
		{
			typeCheck<Asset, T>();
			if (!initialized)
			{
				init();
			}
			return std::dynamic_pointer_cast<T>(Load(path, std::make_shared<T>()));
		}

		template <typename T>
		static auto loadExternalAsset(const std::string& path) -> std::shared_ptr<T>
		{
			typeCheck<Asset, T>();

			if (path.rfind(':', 0) == 0)
			{
				log_error("cannot load external asset while providing a local one");
				return nullptr;
			}

			createExternalEntry(path);

			return load<T>(path);
		}

		template <typename T>
		static auto loadFromMemory(const std::string& name, const char* data, size_t size)
			-> std::shared_ptr<T>
		{
			typeCheck<Asset, T>();

			createImaginaryEntry(name, size);

			if (!initialized)
			{
				init();
			}

			auto asset = std::make_shared<T>();

			auto it = manifest.find(name);
			if (it == manifest.end())
			{
				log_error("asset \"%s\" not found in manifest", name.c_str());
				return nullptr;
			}

			if (assets.find(name) != assets.end())
			{
				return std::dynamic_pointer_cast<T>(assets[name]);
			}

			assets[name] = asset;
			asset->PreSetUp();
			ReadFromMemory(it->second, asset, data);
			return asset;
		}

		static auto load(const std::string& path, std::shared_ptr<Asset> asset)
			-> std::shared_ptr<Asset>;
		static void unload(const std::shared_ptr<Asset>& asset);
		static void unload(const std::string& name);
		static void unloadAll();
		static void init();
		static void mount(const std::string& path, const std::string& mountPoint);
		static void shutdown();

		static void setAssetDirectory(std::string path)
		{
			archivePath = std::move(path);
			init();
		}

	private:
		inline static std::unordered_map<std::string, std::shared_ptr<Asset>> assets;
		inline static std::unordered_map<std::string, ManifestEntry*> manifest;
		inline static std::vector<ManifestEntry*> manifestEntries;
		inline static std::string archivePath = "";

		inline static bool initialized = false;

		static auto openArchive(PHYSFS_Io* io, const char* name, int forWrite,
								int* claimed) -> void*;
		static auto enumerate(void* opaque, const char* dirname,
							  PHYSFS_EnumerateCallback cb, const char* origdir,
							  void* callbackdata) -> PHYSFS_EnumerateCallbackResult;
		static void closeArchive(void* opaque);
		static auto openRead(void* opaque, const char* fnm) -> PHYSFS_Io*;
		static auto openWrite(void* opaque, const char* fnm) -> PHYSFS_Io*;
		static auto remove(void* opaque, const char* fnm) -> int;
		static auto stat(void* opaque, const char* fnm, PHYSFS_Stat* stat) -> int;
		static void buildManifestFromFolder(const std::string& path);

		static void readPhysFS(ManifestEntry* entry, const std::shared_ptr<Asset>& asset,
							   const std::string& path);
		static void readExternal(ManifestEntry* entry,
								 const std::shared_ptr<Asset>& asset,
								 const std::string& path);

		static void readFromMemory(ManifestEntry* entry,
								   const std::shared_ptr<Asset>& asset, const char* data);

		static void createExternalEntry(const std::string& path);
		static void createImaginaryEntry(const std::string& path, size_t size);

		inline static const PHYSFS_Archiver EapkArchiver = {
			.version = 0,
			.info = {.extension = "eapk",
					 .description = "Espresso Asset Package",
					 .author = "Espresso",
					 .url = "",
					 .supportsSymlinks = 0},
			.openArchive = openArchive,
			.enumerate = enumerate,
			.openRead = openRead,
			.openWrite = openWrite,
			.openAppend = openWrite,
			.remove = remove,
			.mkdir = remove,
			.stat = stat,
			.closeArchive = closeArchive,
		};
	};
}