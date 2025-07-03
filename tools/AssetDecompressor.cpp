#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <vector>

struct BundleHeader
{
	char magic[5] = {'B', 'N', 'D', 'L', '\0'}; // signature
	uint8_t version = 1;
	uint64_t indexOffset{0}; // where the index table starts
	uint32_t assetCount;
};

struct FileEntry
{
	std::string path;
	uint64_t hash;
	uint64_t offset;
	uint64_t uncompressedSize;
	uint64_t compressedSize;
	uint8_t compressionType; // 0 = none, 1 = zstd
};

struct Bundle
{
public:
	BundleHeader header;
	std::vector<FileEntry> entries;
};

std::filesystem::path inputRoot;
std::filesystem::path outputRoot;

Bundle readBundle(std::filesystem::path path)
{
	if (path.stem() != "_")
	{
		std::filesystem::create_directories(
			std::filesystem::path(path).replace_extension());
	}

	Bundle bundle;
	BundleHeader header;

	char headerBuffer[5];

	std::ifstream bundleFile(path, std::ios::binary | std::ios::in);
	bundleFile.read(headerBuffer, sizeof(headerBuffer) - 1);

	headerBuffer[4] = '\0';

	if (strcmp(headerBuffer, header.magic) != 0)
	{
		std::cout << "invalid bundle: " << path << "\n";
		return {};
	}

	bundleFile.read(reinterpret_cast<char*>(&header.version), sizeof(header.version));
	bundleFile.read(reinterpret_cast<char*>(&header.indexOffset),
					sizeof(header.indexOffset));
	bundleFile.read(reinterpret_cast<char*>(&header.assetCount),
					sizeof(header.assetCount));

	bundleFile.seekg((long)header.indexOffset);

	for (size_t i = 0; i < header.assetCount; i++)
	{
		short strSize;
		bundleFile.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));

		FileEntry entry;
		entry.path.resize(strSize);
		bundleFile.read((char*)entry.path.data(), strSize);

		bundleFile.read(reinterpret_cast<char*>(&entry.hash), sizeof(entry.hash));
		bundleFile.read(reinterpret_cast<char*>(&entry.offset), sizeof(entry.offset));
		bundleFile.read(reinterpret_cast<char*>(&entry.uncompressedSize),
						sizeof(entry.uncompressedSize));
		bundleFile.read(reinterpret_cast<char*>(&entry.compressedSize),
						sizeof(entry.compressedSize));
		bundleFile.read(reinterpret_cast<char*>(&entry.compressionType),
						sizeof(entry.compressionType));

		// writing asset files
		bundleFile.seekg((long)entry.offset);

		std::filesystem::create_directories((inputRoot.stem() / entry.path).remove_filename());
		std::ofstream output((inputRoot.stem() / entry.path), std::ios::binary);
		
		char* contentBuf = new char[entry.compressedSize];

		bundleFile.read(contentBuf, (long)entry.compressedSize);
		output.write(contentBuf, (long)entry.compressedSize);

		output.close();

		bundle.entries.push_back(entry);
	}

	return bundle;
}

int main(int argc, const char** argv)
{
	if (argc < 2)
	{
		std::cout << "expected at least 1 argument\n";
		return 1;
	}

	inputRoot = std::filesystem::canonical(std::string(argv[1]));
	outputRoot = std::filesystem::path(inputRoot.stem());

	if (argc >= 3)
	{
		outputRoot = std::filesystem::canonical(std::string(argv[2]));
	}

	if (std::filesystem::is_directory(inputRoot))
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(inputRoot))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".bundle")
			{
				readBundle(entry.path());
			}
		}
	}
	else
	{
		readBundle(inputRoot);
	}

	return 0;
}