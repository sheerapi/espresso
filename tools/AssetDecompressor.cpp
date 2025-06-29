#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <zstd.h>

struct ManifestInfo
{
	uint64_t offset;
	uint64_t compressedSize;
	uint64_t uncompressedSize;
	int64_t CreateTime;
	int64_t LastAccessTime;
	int64_t LastWriteTime;
};

uint64_t manifestOffset;
uint64_t manifestSize;

auto readManifest(std::ifstream& archive) -> std::map<std::string, ManifestInfo>
{
	char header[sizeof("EAPK") - 1];
	archive.read(header, sizeof("EAPK") - 1);
	if (std::string(header, sizeof("EAPK") - 1) != "EAPK")
	{
		throw std::runtime_error("invalid archive");
	}

	archive.ignore(1);

	archive.read(reinterpret_cast<char*>(&manifestOffset), sizeof(manifestOffset));
	archive.read(reinterpret_cast<char*>(&manifestSize), sizeof(manifestSize));

	archive.seekg((long)manifestOffset);
	std::vector<char> compressedManifest(manifestSize);
	archive.read(compressedManifest.data(), (long)manifestSize);

	size_t decompressedSize =
		ZSTD_getFrameContentSize(compressedManifest.data(), manifestSize);
	std::vector<char> manifestData(decompressedSize);
	ZSTD_decompress(manifestData.data(), decompressedSize, compressedManifest.data(),
					manifestSize);

	std::map<std::string, ManifestInfo> manifest;
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

		ManifestInfo info;
		memcpy(&info.offset, data, sizeof(info.offset));
		data += sizeof(info.offset);
		memcpy(&info.compressedSize, data, sizeof(info.compressedSize));
		data += sizeof(info.compressedSize);
		memcpy(&info.uncompressedSize, data, sizeof(info.uncompressedSize));
		data += sizeof(info.uncompressedSize);

		memcpy(&info.CreateTime, data, sizeof(info.CreateTime));
		data += sizeof(info.CreateTime);

		memcpy(&info.LastAccessTime, data, sizeof(info.LastAccessTime));
		data += sizeof(info.LastAccessTime);

		memcpy(&info.LastWriteTime, data, sizeof(info.LastWriteTime));
		data += sizeof(info.LastWriteTime);

		manifest[path] = info;
	}

	return manifest;
}

auto decompressFile(std::ifstream& archive, uint64_t offset, uint64_t compressedSize)
	-> std::vector<char>
{
	archive.seekg(offset);
	ZSTD_DStream* dstream = ZSTD_createDStream();
	ZSTD_initDStream(dstream);

	ZSTD_DCtx_setParameter(dstream, ZSTD_d_windowLogMax, 23);

	const size_t InBufSize = ZSTD_DStreamInSize();
	const size_t OutBufSize = ZSTD_DStreamOutSize();
	std::vector<char> inBuf(InBufSize);
	std::vector<char> outBuf(OutBufSize);
	std::vector<char> result;

	size_t totalRead = 0;
	size_t lastResult = 0;

	while (totalRead < compressedSize)
	{
		// Calculate remaining bytes to read for this file
		size_t remaining = compressedSize - totalRead;
		size_t toRead = std::min(remaining, InBufSize);

		archive.read(inBuf.data(), toRead);
		size_t bytesRead = archive.gcount();
		if (bytesRead == 0)
			break;
		totalRead += bytesRead;

		ZSTD_inBuffer input = {inBuf.data(), bytesRead, 0};

		// Decompress until we've processed all read bytes
		while (input.pos < input.size)
		{
			ZSTD_outBuffer output = {outBuf.data(), OutBufSize, 0};
			lastResult = ZSTD_decompressStream(dstream, &output, &input);

			if (ZSTD_isError(lastResult))
			{
				throw std::runtime_error("decompression error: " +
										 std::string(ZSTD_getErrorName(lastResult)));
			}

			result.insert(result.end(), outBuf.data(), outBuf.data() + output.pos);
		}
	}

	// Final validation
	if (totalRead != compressedSize)
	{
		throw std::runtime_error("didn't read all compressed bytes for file");
	}

	if (lastResult != 0)
	{
		throw std::runtime_error("decompression didn't complete");
	}

	ZSTD_freeDStream(dstream);
	return result;
}

auto main(int argc, char* argv[]) -> int
{
	if (argc < 2)
	{
		std::cerr << "usage: " << argv[0] << " archive.eapk file_path\n";
		return 1;
	}

	std::ifstream archive(argv[1], std::ios::binary);
	auto manifest = readManifest(archive);

	if (argc == 2)
	{
		// List all manifest entries as a nicely formatted table with all fields

		std::cout << std::left << std::setw(40) << "Path" << std::setw(20)
				  << "compressed size" << std::setw(20) << "uncompressed size"
				  << std::setw(15) << "offset" << std::setw(25) << "creation time"
				  << std::setw(20) << "last write time" << '\n';

		std::cout << std::string(140, '-') << '\n';

		std::cout << std::left << std::setw(40) << "manifest" << std::setw(20)
				  << manifestSize << std::setw(20) << "<Not defined>" << std::setw(20)
				  << manifestOffset << '\n';

		for (const auto& [path, info] : manifest)
		{
			char creationTime[20];
			strftime(creationTime, 20, "%d-%m-%Y %H:%M:%S", localtime(&info.CreateTime));

			char lastWriteTime[20];
			strftime(lastWriteTime, 20, "%d-%m-%Y %H:%M:%S",
					 localtime(&info.LastWriteTime));

			std::cout << std::left << std::setw(40) << path << std::setw(20)
					  << info.compressedSize << std::setw(20) << info.uncompressedSize
					  << std::setw(15) << info.offset << std::setw(25) << creationTime
					  << std::setw(20) << lastWriteTime << '\n';
		}
		return 0;
	}

	if (strcmp(argv[2], "--all") == 0)
	{
		auto outputDir = std::filesystem::path(argv[1]).stem();
		std::filesystem::create_directory(outputDir);

		for (const auto& [path, info] : manifest)
		{
			auto data = decompressFile(archive, info.offset, info.compressedSize);

			// Create the output directory structure
			auto outputPath = outputDir / path;
			std::filesystem::create_directories(outputPath.parent_path());

			std::ofstream outputFile(outputPath, std::ios::binary);
			outputFile.write(data.data(), (long)data.size());

			std::cout << "wrote " << (long)data.size() << " bytes to " << outputPath
					  << '\n';
		}

		return 0;
	}

	if (manifest.find(argv[2]) == manifest.end())
	{
		std::cerr << "file not found in archive\n";
		return 1;
	}

	auto data = decompressFile(archive, manifest[argv[2]].offset,
							   manifest[argv[2]].compressedSize);

	std::ofstream outputFile(argv[2], std::ios::binary);
	outputFile.write(data.data(), (long)data.size());
	return 0;
}