#include "platform/AssetManager.h"
#include "core/EventManager.h"
#include "physfs.h"
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <filesystem>
#include <sys/stat.h>
#include <algorithm>

namespace core
{
	namespace internals
	{
		auto zstdFileRead(PHYSFS_Io* io, void* buf, PHYSFS_uint64 len) -> PHYSFS_sint64;
		auto zstdFileSeek(PHYSFS_Io* io, PHYSFS_uint64 offset) -> int;
		auto zstdFileTell(PHYSFS_Io* io) -> PHYSFS_sint64;
		auto zstdFileLength(PHYSFS_Io* io) -> PHYSFS_sint64;
		void zstdFileClose(PHYSFS_Io* io);
	}

	void AssetManager::init()
	{
		if (initialized)
		{
			return;
		}

		initialized = true;
		std::vector<const char*> args;

		PHYSFS_init(nullptr);
		PHYSFS_setWriteDir((std::filesystem::current_path() / "assets").c_str());

		EventManager::triggerEvent("assets.mount");

		if (PHYSFS_registerArchiver(&EapkArchiver) == 0)
		{
			log_error("failed to register game archiver: %s",
					  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return;
		}

		if (archivePath == "")
		{
			archivePath = std::filesystem::current_path() / "assets";
		}

		mount(std::filesystem::path(archivePath), "/");
		mount(archivePath + ".eapk", "/");
		mount(std::filesystem::current_path() / "assets.eapk", "/");
	}

	auto AssetManager::load(const std::string& path, std::shared_ptr<Asset> asset)
		-> std::shared_ptr<Asset>
	{
		if (!initialized)
		{
			init();
		}

		bool physfs = path.rfind(':', 0) == 0;

		auto pathNoExt = std::filesystem::path(path.substr(physfs ? 2 : 0))
							 .replace_extension()
							 .string();

		auto it = manifest.find(pathNoExt);
		if (it == manifest.end())
		{
			log_error("asset \"%s\" not found in manifest", path.c_str());
			return nullptr;
		}

		if (assets.find(pathNoExt) != assets.end())
		{
			return assets[pathNoExt];
		}

		assets[path.substr(physfs ? 1 : 0)] = asset;
		asset->preSetUp();

		if (physfs)
		{
			readPhysFS(it->second, asset, path.substr(2));
		}
		else
		{
			readExternal(it->second, asset, path);
		}

		return asset;
	}

	void AssetManager::mount(const std::string& path, const std::string& mountPoint)
	{
		if (!initialized)
		{
			init();
		}

		if (!std::filesystem::exists(path))
		{
			return;
		}

		size_t currentLoaded = manifest.size();

		if (std::filesystem::is_directory(path))
		{
			buildManifestFromFolder(path);
		}

		if (PHYSFS_mount(path.c_str(), mountPoint.c_str(), 1) == 0)
		{
			log_error("failed to mount asset archive \"%s\": %s", path.c_str(),
					  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return;
		}

		log_trace("mounted asset archive %s at %s (%d assets loaded)",
				  std::filesystem::relative(path).c_str(), mountPoint.c_str(),
				  manifest.size() - currentLoaded);
	}

	void AssetManager::buildManifestFromFolder(const std::string& path)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (entry.is_regular_file())
			{
				auto relativePath =
					std::filesystem::relative(entry.path().string(), path).string();

				struct statx s;
				statx(AT_FDCWD, entry.path().c_str(), 0, STATX_BASIC_STATS | STATX_BTIME,
					  &s);

				auto pathNoExt =
					std::filesystem::path(relativePath).replace_extension().string();

				manifest[pathNoExt] = new ManifestEntry();
				manifest[pathNoExt]->path = relativePath;
				manifest[pathNoExt]->offset = 0;
				manifest[pathNoExt]->size = s.stx_size;
				manifest[pathNoExt]->realSize = s.stx_size;
				manifest[pathNoExt]->createTime = s.stx_btime.tv_sec;
				manifest[pathNoExt]->lastAccessTime = s.stx_atime.tv_sec;
				manifest[pathNoExt]->lastWriteTime = s.stx_mtime.tv_sec;

				manifestEntries.push_back(manifest[pathNoExt]);
			}
		}
	}

	void AssetManager::shutdown()
	{
		if (!initialized)
		{
			return;
		}

		PHYSFS_deinit();

		for (auto& entry : manifestEntries)
		{
			auto pathNoExt =
				std::filesystem::path(entry->path).replace_extension().string();
			manifest.erase(pathNoExt);

			delete entry;
			entry = nullptr;
		}

		manifest.clear();
		manifestEntries.clear();
		assets.clear();
	}

	auto AssetManager::openArchive(PHYSFS_Io* io, const char* name, int forWrite,
								   int* claimed) -> void*
	{
		uint64_t pos = 0;

		std::array<char, sizeof("EAPK") - 1> header;
		io->read(io, header.data(), sizeof("EAPK") - 1);
		pos += sizeof("EAPK") - 1;

		if (std::string(header.data(), sizeof("EAPK") - 1) != "EAPK")
		{
			*claimed = 0;
			return nullptr;
		}

		*claimed = 1;

		io->seek(io, ++pos);

		uint64_t manifestOffset = 0;
		uint64_t manifestSize = 0;

		io->read(io, &manifestOffset, sizeof(manifestOffset));
		pos += sizeof(manifestOffset);

		io->read(io, &manifestSize, sizeof(manifestSize));
		pos += sizeof(manifestSize);

		io->seek(io, manifestOffset);
		std::vector<char> compressedManifest(manifestSize);
		io->read(io, compressedManifest.data(), manifestSize);

		size_t decompressedSize =
			ZSTD_getFrameContentSize(compressedManifest.data(), manifestSize);
		std::vector<char> manifestData(decompressedSize);
		ZSTD_decompress(manifestData.data(), decompressedSize, compressedManifest.data(),
						manifestSize);

		auto* instance = new EapkInstanceData{
			.io = io,
			.manifest = {},
			.directories = {},
		};
		auto* data = manifestData.data();
		uint32_t count;
		memcpy(&count, data, sizeof(count));
		data += sizeof(count);

		for (uint32_t i = 0; i < count; i++)
		{
			uint16_t pathLen;
			memcpy(&pathLen, data, sizeof(pathLen));
			data += sizeof(pathLen);

			std::string path(data, data + pathLen);
			data += pathLen;

			ManifestEntry info;
			info.path = path;

			memcpy(&info.offset, data, sizeof(info.offset));
			data += sizeof(info.offset);
			memcpy(&info.size, data, sizeof(info.size));
			data += sizeof(info.size);
			memcpy(&info.realSize, data, sizeof(info.realSize));
			data += sizeof(info.realSize);

			memcpy(&info.createTime, data, sizeof(info.createTime));
			data += sizeof(info.createTime);

			memcpy(&info.lastAccessTime, data, sizeof(info.lastAccessTime));
			data += sizeof(info.lastAccessTime);

			memcpy(&info.lastWriteTime, data, sizeof(info.lastWriteTime));
			data += sizeof(info.lastWriteTime);

			instance->manifest[info.path] = info;
		}

		for (auto& [path, info] : instance->manifest)
		{
			AssetManager::manifest
				[std::filesystem::path(path).replace_extension().string()] = &info;

			auto separator = info.path.find_last_of('/');
			if (separator != std::string::npos)
			{
				auto dir = info.path.substr(0, separator);
				auto file = info.path.substr(separator + 1);

				instance->directories[dir].push_back(file);
			}
			else
			{
				instance->directories["/"].push_back(info.path);
			}
		}

		return instance;
	}

	auto AssetManager::enumerate(void* opaque, const char* dirname,
								 PHYSFS_EnumerateCallback cb, const char* origdir,
								 void* callbackdata) -> PHYSFS_EnumerateCallbackResult
	{
		auto* data = static_cast<EapkInstanceData*>(opaque);

		auto it = data->directories.find(dirname);
		if (it == data->directories.end())
		{
			PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
			return PHYSFS_ENUM_ERROR;
		}

		for (const auto& file : it->second)
		{
			if (cb(callbackdata, origdir, file.c_str()) != PHYSFS_ENUM_OK)
			{
				return PHYSFS_ENUM_ERROR;
			}
		}

		return PHYSFS_ENUM_OK;
	}

	void AssetManager::closeArchive(void* opaque)
	{
		auto* data = static_cast<EapkInstanceData*>(opaque);

		data->io->flush(data->io);
		data->io->destroy(data->io);

		for (auto& [path, info] : data->manifest)
		{
			AssetManager::manifest.erase(path);
		}

		delete data;
	}

	auto AssetManager::openRead(void* opaque, const char* fnm) -> PHYSFS_Io*
	{
		auto* data = static_cast<EapkInstanceData*>(opaque);

		auto it = data->manifest.find(fnm);
		if (it == data->manifest.end())
		{
			PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
			return nullptr;
		}

		auto& info = it->second;

		auto* handle = new ZstdFileHandle();
		handle->io.version = 0;
		handle->io.read = internals::zstdFileRead;
		handle->io.seek = internals::zstdFileSeek;
		handle->io.tell = internals::zstdFileTell;
		handle->io.length = internals::zstdFileLength;
		handle->io.destroy = internals::zstdFileClose;
		handle->io.duplicate = nullptr; // Not needed for read-only

		handle->arcIo = data->io->duplicate(data->io); // Thread-safe handle
		handle->arcIo->seek(handle->arcIo, info.offset);
		handle->startOffset = info.offset;
		handle->compressedSize = info.size;
		handle->decompressedSize = info.realSize;

		// Initialize Zstd
		handle->dStream = ZSTD_createDStream();
		handle->inBufSize = ZSTD_DStreamInSize();
		handle->outBufSize = ZSTD_DStreamOutSize();
		handle->inBuf = (char*)malloc(handle->inBufSize);
		handle->outBuf = (char*)malloc(handle->outBufSize);
		handle->outBufAvailable = 0;
		handle->decompressedPos = 0;

		return (PHYSFS_Io*)handle;
	}

	auto AssetManager::openWrite(void* opaque, const char* fnm) -> PHYSFS_Io*
	{
		PHYSFS_setErrorCode(PHYSFS_ERR_OPEN_FOR_WRITING);
		return nullptr;
	}

	auto AssetManager::remove(void* opaque, const char* fnm) -> int
	{
		PHYSFS_setErrorCode(PHYSFS_ERR_OPEN_FOR_WRITING);
		return 0;
	}

	auto AssetManager::stat(void* opaque, const char* fnm, PHYSFS_Stat* stat) -> int
	{
		return 0;
	}

	auto internals::zstdFileRead(PHYSFS_Io* io, void* buf, PHYSFS_uint64 len)
		-> PHYSFS_sint64
	{
		auto* handle = (ZstdFileHandle*)io;
		size_t total_read = 0;

		while (len > 0)
		{
			// If output buffer is empty, decompress more data
			if (handle->outBufAvailable == 0)
			{
				// Read compressed chunk from archive
				size_t bytes_read =
					handle->arcIo->read(handle->arcIo, handle->inBuf, handle->inBufSize);
				if (bytes_read == 0)
				{
					break; // End of compressed data
				}

				// Prepare Zstd input buffer
				handle->zstdInput.src = handle->inBuf;
				handle->zstdInput.size = bytes_read;
				handle->zstdInput.pos = 0;

				// Decompress into output buffer
				handle->zstdOutput.dst = handle->outBuf;
				handle->zstdOutput.size = handle->outBufSize;
				handle->zstdOutput.pos = 0;

				size_t ret = ZSTD_decompressStream(handle->dStream, &handle->zstdOutput,
												   &handle->zstdInput);
				if (ZSTD_isError(ret) != 0U)
				{
					return -1; // Decompression error
				}

				handle->outBufAvailable = handle->zstdOutput.pos;
			}

			// Copy decompressed data to user buffer
			size_t to_copy =
				(len < handle->outBufAvailable) ? len : handle->outBufAvailable;
			memcpy(buf, handle->outBuf, to_copy);
			buf = (char*)buf + to_copy;
			len -= to_copy;
			total_read += to_copy;
			handle->outBufAvailable -= to_copy;
			handle->decompressedPos += to_copy;
		}

		return (PHYSFS_sint64)total_read;
	}

	auto internals::zstdFileSeek(PHYSFS_Io* io, PHYSFS_uint64 offset) -> int
	{
		auto* handle = (ZstdFileHandle*)io;
		if (offset > handle->decompressedSize)
		{
			return 0; // Invalid
		}

		// If seeking backward, reset decompression to start
		if (offset < handle->decompressedPos)
		{
			ZSTD_initDStream(handle->dStream); // Reset decompression
			handle->arcIo->seek(handle->arcIo, handle->startOffset);
			handle->decompressedPos = 0;
			handle->outBufAvailable = 0;
		}

		// Read and discard data until we reach the desired offset
		char discard[4096];
		while (handle->decompressedPos < offset)
		{
			size_t to_read = (offset - handle->decompressedPos < 4096)
								 ? offset - handle->decompressedPos
								 : 4096;
			if (zstdFileRead(io, discard, to_read) != (PHYSFS_sint64)to_read)
			{
				return 0;
			}
		}

		return 1;
	}

	auto internals::zstdFileTell(PHYSFS_Io* io) -> PHYSFS_sint64
	{
		auto* handle = (ZstdFileHandle*)io;
		return (PHYSFS_sint64)handle->decompressedPos;
	}

	auto internals::zstdFileLength(PHYSFS_Io* io) -> PHYSFS_sint64
	{
		auto* handle = (ZstdFileHandle*)io;
		return (PHYSFS_sint64)handle->decompressedSize;
	}

	void internals::zstdFileClose(PHYSFS_Io* io)
	{
		auto* handle = (ZstdFileHandle*)io;
		ZSTD_freeDStream(handle->dStream);
		handle->arcIo->destroy(handle->arcIo);
		free(handle->inBuf);
		free(handle->outBuf);
		delete handle;
	}

	void AssetManager::readPhysFS(ManifestEntry* entry,
								  const std::shared_ptr<Asset>& asset,
								  const std::string& path)
	{
		auto* io = PHYSFS_openRead(entry->path.c_str());
		if (io == nullptr)
		{
			log_error("failed to open asset file \"%s\": %s", entry->path.c_str(),
					  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return;
		}

		auto size = PHYSFS_fileLength(io);
		char* buffer = (char*)malloc(size);

		auto bytesRead = PHYSFS_readBytes(io, buffer, size);
		if (bytesRead != size)
		{
			log_error("failed to read asset file \"%s\": %s", entry->path.c_str(),
					  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			PHYSFS_close(io);
			free(buffer);
			return;
		}

		PHYSFS_close(io);
		auto success = asset->load(buffer, size);
		asset->setUp();
		free(buffer);
		assets[path] = asset;

		asset->_path = entry->path;
		asset->_size = size;
		asset->loaded = true;
		asset->succeeded = success;
		for (const auto& callback : asset->_loadCallbacks)
		{
			callback(asset.get(), success);
		}
	}

	void AssetManager::readExternal(ManifestEntry* entry,
									const std::shared_ptr<Asset>& asset,
									const std::string& path)
	{
		auto size = entry->realSize;
		char* buffer = (char*)malloc(size);

		int fd = open(path.c_str(), O_RDONLY);
		if (fd < 0)
		{
			log_error("failed to open asset file \"%s\": %s", entry->path.c_str(),
					  strerror(errno));
			free(buffer);
			return;
		}

		auto bytesRead = read(fd, buffer, size);
		if ((unsigned long)bytesRead != size)
		{
			log_error("failed to read asset file \"%s\": %s", entry->path.c_str(),
					  strerror(errno));
			close(fd);
			free(buffer);
			return;
		}

		close(fd);
		auto success = asset->load(buffer, size);
		free(buffer);

		if (!success)
		{
			log_error("failed to load asset file \"%s\"", entry->path.c_str());
			return;
		}

		asset->setUp();
		assets[path] = asset;

		asset->_path = entry->path;
		asset->_size = size;
		asset->loaded = true;
		asset->succeeded = success;
		for (const auto& callback : asset->_loadCallbacks)
		{
			callback(asset.get(), success);
		}
	}

	void AssetManager::createExternalEntry(const std::string& path)
	{
		if (!std::filesystem::exists(path))
		{
			log_error("asset file \"%s\" does not exist", path.c_str());
			return;
		}

		auto pathNoExt = std::filesystem::path(path).replace_extension().string();
		if (manifest.find(pathNoExt) == manifest.end())
		{
			auto* info = new ManifestEntry();

			struct statx s;
			statx(AT_FDCWD, path.c_str(), 0, STATX_BASIC_STATS | STATX_BTIME, &s);

			info->path = path;
			info->offset = 0;
			info->size = s.stx_size;
			info->realSize = s.stx_size;
			info->createTime = s.stx_btime.tv_sec;
			info->lastAccessTime = s.stx_atime.tv_sec;
			info->lastWriteTime = s.stx_mtime.tv_sec;

			manifest[pathNoExt] = info;
			manifestEntries.push_back(info);
		}
	}

	void AssetManager::createImaginaryEntry(const std::string& path, size_t size)
	{
		if (assets.find(path) == assets.end())
		{
			auto* info = new ManifestEntry();

			info->path = path;
			info->offset = 0;
			info->size = size;
			info->realSize = size;
			info->createTime = 0;
			info->lastAccessTime = 0;
			info->lastWriteTime = 0;
			info->real = false;

			manifest[path] = info;
			manifestEntries.push_back(info);
		}
	}

	void AssetManager::unload(const std::shared_ptr<Asset>& asset)
	{
		for (auto it = assets.begin(); it != assets.end();)
		{
			if (it->second == asset)
			{
				it = assets.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void AssetManager::unload(const std::string& name)
	{
		for (auto it = assets.begin(); it != assets.end();)
		{
			if (it->first == name)
			{
				it = assets.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void AssetManager::unloadAll()
	{
		assets.clear();
	}

	void AssetManager::readFromMemory(ManifestEntry* entry,
									  const std::shared_ptr<Asset>& asset,
									  const char* data)
	{
		auto success = asset->loadFromMemory(const_cast<char*>(data), entry->realSize);
		if (!success)
		{
			log_error("failed to load asset file \"%s\"", entry->path.c_str());
			return;
		}

		asset->setUp();
		assets[entry->path] = asset;

		asset->_path = entry->path;
		asset->_size = entry->realSize;
		asset->loaded = true;
		asset->succeeded = success;
		for (const auto& callback : asset->_loadCallbacks)
		{
			callback(asset.get(), success);
		}
	}

	Asset::~Asset()
	{
		unload();
		AssetManager::unload(std::filesystem::path(_path).replace_extension());
	}
}