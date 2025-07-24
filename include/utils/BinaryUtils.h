#pragma once
#include "platform/EnvironmentInfo.h"
#include "BinaryWriter.h"
#include "BinaryReader.h"

namespace utils
{
	template <typename T> void writeMagic(BinaryWriter& writer, T magic)
	{
		writer.write(magic);
	}

	template <typename T> auto validateMagic(BinaryReader& reader, T expectedMagic) -> bool
	{
		T magic = reader.read<T>();
		return magic == expectedMagic;
	}

	void writeVersion(BinaryWriter& writer, const platform::Version& version)
	{
		writer.write(version.major).write(version.minor).write(version.patch);
	}

	auto readVersion(BinaryReader& reader) -> platform::Version
	{
		return {reader.read<uint16_t>(), reader.read<uint16_t>(), reader.read<uint16_t>()};
	}
}