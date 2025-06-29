#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <zstd.h>

namespace fs = std::filesystem;

struct ManifestEntry
{
	std::string Path;
	uint64_t Offset;
	uint64_t CompressedSize;
	uint64_t UncompressedSize;
	int64_t CreateTime;
	int64_t LastAccessTime;
	int64_t LastWriteTime;
};

void compressFile(std::ofstream& archive, const fs::path& filePath, int level)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file)
	{
		throw std::runtime_error("can't open input file");
	}

	ZSTD_CStream* cstream = ZSTD_createCStream();
	ZSTD_initCStream(cstream, level);

	ZSTD_CCtx_setParameter(cstream, ZSTD_c_windowLog, 23);

	const long InBufSize = (long)ZSTD_CStreamInSize();
	const long OutBufSize = (long)ZSTD_CStreamOutSize();
	std::vector<char> inBuf(InBufSize);
	std::vector<char> outBuf(OutBufSize);

	size_t remaining;
	do
	{
		file.read(inBuf.data(), InBufSize);
		ZSTD_inBuffer input = {inBuf.data(), static_cast<size_t>(file.gcount()), 0};
		while (input.pos < input.size)
		{
			ZSTD_outBuffer output = {outBuf.data(), static_cast<size_t>(OutBufSize), 0};
			remaining = ZSTD_compressStream2(cstream, &output, &input, ZSTD_e_continue);
			archive.write(outBuf.data(), (long)output.pos);
		}
	} while (file);

	// Fixed final flush loop
	ZSTD_inBuffer input = {nullptr, 0, 0}; // Empty input buffer for final flush
	do
	{
		ZSTD_outBuffer output = {outBuf.data(), static_cast<size_t>(OutBufSize), 0};
		remaining = ZSTD_compressStream2(cstream, &output, &input, ZSTD_e_end);
		archive.write(outBuf.data(), (long)output.pos);
	} while (remaining > 0);

	ZSTD_freeCStream(cstream);
}

void compressDirectory(const fs::path& inputDir, const fs::path& outputFile,
					   int level = 3)
{
	std::ofstream archive(outputFile, std::ios::binary);
	archive.write("EAPK\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
				  sizeof(uint64_t) * 3); // Header placeholder

	std::vector<ManifestEntry> manifest;

	for (const auto& entry : fs::recursive_directory_iterator(inputDir))
	{
		if (entry.is_regular_file())
		{
			fs::path relPath = fs::relative(entry.path(), inputDir);
			uint64_t offset = static_cast<uint64_t>(archive.tellp());
			struct statx s;
			statx(AT_FDCWD, entry.path().c_str(), 0, STATX_BASIC_STATS | STATX_BTIME, &s);

			compressFile(archive, entry.path(), level);
			uint64_t compressedSize = static_cast<uint64_t>(archive.tellp()) - offset;

			manifest.push_back({relPath.string(), offset, compressedSize, s.stx_size,
								s.stx_btime.tv_sec, s.stx_atime.tv_sec,
								s.stx_mtime.tv_sec});
		}
	}

	// Serialize manifest
	std::stringstream manifestStream;
	uint32_t count = manifest.size();
	manifestStream.write(reinterpret_cast<char*>(&count), sizeof(count));

	for (const auto& entry : manifest)
	{
		uint16_t len = entry.Path.size();
		manifestStream.write(reinterpret_cast<const char*>(&len), sizeof(len));
		manifestStream.write(entry.Path.data(), len);
		manifestStream.write(reinterpret_cast<const char*>(&entry.Offset),
							 sizeof(entry.Offset));
		manifestStream.write(reinterpret_cast<const char*>(&entry.CompressedSize),
							 sizeof(entry.CompressedSize));
		manifestStream.write(reinterpret_cast<const char*>(&entry.UncompressedSize),
							 sizeof(entry.UncompressedSize));
		manifestStream.write(reinterpret_cast<const char*>(&entry.CreateTime),
							 sizeof(entry.CreateTime));
		manifestStream.write(reinterpret_cast<const char*>(&entry.LastAccessTime),
							 sizeof(entry.LastAccessTime));
		manifestStream.write(reinterpret_cast<const char*>(&entry.LastWriteTime),
							 sizeof(entry.LastWriteTime));
	}

	std::string manifestData = manifestStream.str();
	size_t compressedSize = ZSTD_compressBound(manifestData.size());
	std::vector<char> compressedManifest(compressedSize);
	compressedSize = ZSTD_compress(compressedManifest.data(), compressedSize,
								   manifestData.data(), manifestData.size(), level);

	// Write manifest and update header
	uint64_t manifestOffset = archive.tellp();
	archive.write(compressedManifest.data(), (long)compressedSize);

	archive.seekp(0);
	archive.write("EAPK", sizeof("EAPK"));
	archive.write(reinterpret_cast<char*>(&manifestOffset), sizeof(manifestOffset));
	archive.write(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
}

auto main(int argc, char** argv) -> int
{
	if (argc < 2)
	{
		std::cerr << "usage: eapkc <input_directory>" << '\n';
		return 1;
	}

	compressDirectory(argv[1], "assets.eapk", 10);
	return 0;
}