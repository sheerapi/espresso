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

	struct Version
	{
		uint16_t major;
		uint16_t minor;

		auto operator==(const Version& other) const -> bool
		{
			return major == other.major && minor == other.minor;
		}

		auto operator>=(const Version& other) const -> bool
		{
			return major > other.major || (major == other.major && minor >= other.minor);
		}
	};

	void writeVersion(BinaryWriter& writer, const Version& version)
	{
		writer.write(version.major).write(version.minor);
	}

	auto readVersion(BinaryReader& reader) -> Version
	{
		return {reader.read<uint16_t>(), reader.read<uint16_t>()};
	}
}