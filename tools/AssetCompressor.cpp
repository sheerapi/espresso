#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <vector>

struct BundleHeader
{
	char magic[4] = {'B', 'N', 'D', 'L'}; // signature
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
	std::string path;
	bool pure{true};
	std::vector<std::string> contents;
};

std::filesystem::path inputRoot;
std::filesystem::path outputRoot;

std::vector<Bundle> bundles;
std::vector<FileEntry> index;

void createBundle(const std::string& path);

auto createFileEntry(const std::string& path) -> FileEntry
{
	FileEntry entry;
	entry.path = std::filesystem::relative(path, inputRoot).string();
	entry.hash = std::hash<std::string>{}(entry.path);

	struct stat fileStat;
	stat(path.c_str(), &fileStat);

	entry.uncompressedSize = fileStat.st_size;
	entry.compressionType = 0;
	entry.compressedSize = entry.uncompressedSize;
	return entry;
}

void writeBundle(const Bundle& bundle)
{
	std::ofstream bundleFile(bundle.path, std::ios::binary | std::ios::out);
	long indexOffsetPos;

	BundleHeader header;
	header.assetCount = bundle.contents.size();

	bundleFile.write(header.magic, sizeof(header.magic));
	bundleFile.write(reinterpret_cast<const char*>(&header.version),
					 sizeof(header.version));
	indexOffsetPos = bundleFile.tellp();

	bundleFile.write(reinterpret_cast<const char*>(&header.indexOffset),
					 sizeof(header.indexOffset));
	bundleFile.write(reinterpret_cast<const char*>(&header.assetCount),
					 sizeof(header.assetCount));

	index.clear();
	index.reserve(header.assetCount);

	for (const auto& file : bundle.contents)
	{
		auto entry = createFileEntry(file);
		entry.offset = bundleFile.tellp();

		std::ifstream fileStream(file, std::ios::binary | std::ios::in);
		char buffer[4096];

		while (fileStream.read(buffer, sizeof(buffer)))
		{
			bundleFile.write(buffer, sizeof(buffer));
		}

		if (fileStream.gcount() > 0)
		{
			bundleFile.write(buffer, fileStream.gcount());
		}

		index.push_back(entry);
	}

	header.indexOffset = bundleFile.tellp();

	for (const auto& entry : index)
	{
		auto strSize = (short)entry.path.size();

		bundleFile.write(reinterpret_cast<const char*>(&strSize),
						 sizeof(strSize));
		bundleFile.write(reinterpret_cast<const char*>(entry.path.c_str()),
						 entry.path.size());
		bundleFile.write(reinterpret_cast<const char*>(&entry.hash), sizeof(entry.hash));
		bundleFile.write(reinterpret_cast<const char*>(&entry.offset),
						 sizeof(entry.offset));
		bundleFile.write(reinterpret_cast<const char*>(&entry.uncompressedSize),
						 sizeof(entry.uncompressedSize));
		bundleFile.write(reinterpret_cast<const char*>(&entry.compressedSize),
						 sizeof(entry.compressedSize));
		bundleFile.write(reinterpret_cast<const char*>(&entry.compressionType),
						 sizeof(entry.compressionType));
	}

	bundleFile.seekp(indexOffsetPos);
	bundleFile.write(reinterpret_cast<const char*>(&header.indexOffset),
					 sizeof(header.indexOffset));

	bundleFile.close();
}

auto main(int argc, const char** argv) -> int
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

	createBundle(inputRoot);

	for (auto& bundle : bundles)
	{
		writeBundle(bundle);
	}

	return 0;
}

void createBundle(const std::string& path)
{
	Bundle bundle;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			bundle.pure = false;
			createBundle(entry.path()); // Recurse
		}
		else
		{
			bundle.contents.push_back(entry.path().string());
		}
	}

	// done to basically transpose the base folder structure into the current folder
	// to not clutter the asset folder with bundles
	std::filesystem::path relative = std::filesystem::relative(path, inputRoot);
	std::filesystem::path outputPath = outputRoot / relative;

	if (bundle.pure)
	{
		outputPath += ".bundle";
	}
	else
	{
		outputPath /= "_.bundle";
	}

	std::filesystem::create_directories(outputPath.parent_path()); // ensure folders exist

	bundle.path = outputPath.string();

	if (bundle.contents.size() > 0)
	{
		bundles.push_back(bundle);
	}
}