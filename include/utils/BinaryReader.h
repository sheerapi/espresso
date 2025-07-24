#pragma once
#include "utils/BinaryWriter.h"
#include <istream>
#include <limits>

namespace utils
{
	class BinaryReader
	{
	private:
		std::istream* _stream;
		bool _ownsStream;
		Endian _endian;

		template <typename T> auto _readValue() -> T
		{
			static_assert(std::is_trivially_copyable_v<T>,
						  "Type must be trivially copyable");

			T value;
			_stream->read(reinterpret_cast<char*>(&value), sizeof(T));

			if (!_stream->good())
			{
				throw std::runtime_error("Read operation failed");
			}

			if constexpr (sizeof(T) > 1)
			{
				if (_needsEndianSwap())
				{
					value = _swapEndian(value);
				}
			}

			return value;
		}

		template <typename T> [[nodiscard]] auto _swapEndian(const T& value) const -> T
		{
			static_assert(std::is_trivially_copyable_v<T>);
			T result;
			const char* src = reinterpret_cast<const char*>(&value);
			char* dst = reinterpret_cast<char*>(&result);

			for (size_t i = 0; i < sizeof(T); ++i)
			{
				dst[i] = src[sizeof(T) - 1 - i];
			}
			return result;
		}

		[[nodiscard]] auto _needsEndianSwap() const -> bool
		{
			if (_endian == Endian::Native)
			{
				return false;
			}

			uint16_t test = 0x0102;
			bool is_little_endian = (*reinterpret_cast<uint8_t*>(&test) == 0x02);

			return (_endian == Endian::Little && !is_little_endian) ||
				   (_endian == Endian::Big && is_little_endian);
		}

	public:
		explicit BinaryReader(std::istream& stream, Endian endian = Endian::Native)
			: _stream(&stream), _ownsStream(false), _endian(endian)
		{
		}

		explicit BinaryReader(const std::string& filename, Endian endian = Endian::Native)
			: _ownsStream(true), _endian(endian)
		{
			auto* file_stream = new std::ifstream(filename, std::ios::binary);
			if (!file_stream->is_open())
			{
				delete file_stream;
				throw std::runtime_error("Failed to open file: " + filename);
			}
			_stream = file_stream;
		}

		~BinaryReader()
		{
			if (_ownsStream)
			{
				delete _stream;
			}
		}

		BinaryReader(const BinaryReader&) = delete;
		auto operator=(const BinaryReader&) -> BinaryReader& = delete;
		BinaryReader(BinaryReader&&) = default;
		auto operator=(BinaryReader&&) -> BinaryReader& = default;

		template <typename T> auto read() -> T
		{
			if constexpr (std::is_arithmetic_v<T>)
			{
				return _readValue<T>();
			}
			else if constexpr (std::is_same_v<std::string, T>)
			{
				readString();
			}
			else if constexpr (HasReadMethodV<T>)
			{
				T obj;
				obj.read(*this);
				return obj;
			}
			else
			{
				static_assert(
					std::is_arithmetic_v<T> || HasReadMethodV<T>,
					"Type must be arithmetic or have a read(BinaryReader&) method");
			}
		}
		auto readString(bool nullTerminated = false) -> std::string
		{
			auto length = _readValue<uint32_t>();

			if (length > std::numeric_limits<uint32_t>::max())
			{
				throw std::runtime_error("String length too large: " +
										 std::to_string(length));
			}

			std::string result(length, '\0');
			_stream->read(result.data(), length);

			if (nullTerminated)
			{
				auto null_byte = _readValue<uint8_t>();
				if (null_byte != 0)
				{
					throw std::runtime_error("Expected null terminator not found");
				}
			}

			if (!_stream->good())
			{
				throw std::runtime_error("String read operation failed");
			}

			return result;
		}

		template <typename T> auto readVector() -> std::vector<T>
		{
			auto size = _readValue<uint32_t>();

			if (size > std::numeric_limits<uint32_t>::max())
			{
				throw std::runtime_error("Vector size too large: " +
										 std::to_string(size));
			}

			std::vector<T> result;
			result.reserve(size);

			for (uint32_t i = 0; i < size; ++i)
			{
				if constexpr (std::is_arithmetic_v<T>)
				{
					result.push_back(_readValue<T>());
				}
				else if constexpr (HasReadMethodV<T>)
				{
					T obj;
					obj.read(*this);
					result.push_back(std::move(obj));
				}
				else
				{
					result.push_back(read<T>());
				}
			}

			return result;
		}

		auto readBuffer(size_t size) -> std::vector<uint8_t>
		{
			std::vector<uint8_t> buffer(size);
			_stream->read(reinterpret_cast<char*>(buffer.data()), (long)size);

			if (!_stream->good())
			{
				throw std::runtime_error("Buffer read operation failed");
			}

			return buffer;
		}

		void readBuffer(void* data, size_t size)
		{
			_stream->read(static_cast<char*>(data), (long)size);
			if (!_stream->good())
			{
				throw std::runtime_error("Buffer read operation failed");
			}
		}

		auto skip(size_t bytes) -> BinaryReader&
		{
			_stream->ignore((long)bytes);
			return *this;
		}

		auto alignTo(size_t boundary) -> BinaryReader&
		{
			size_t pos = _stream->tellg();
			size_t padding = (boundary - (pos % boundary)) % boundary;
			if (padding > 0)
			{
				skip(padding);
			}
			return *this;
		}

		[[nodiscard]] auto tell() const -> size_t
		{
			return _stream->tellg();
		}

		auto seek(size_t pos) -> BinaryReader&
		{
			_stream->seekg((long)pos);
			return *this;
		}

		[[nodiscard]] auto eof() const -> bool
		{
			return _stream->eof();
		}

		[[nodiscard]] auto good() const -> bool
		{
			return _stream->good();
		}

		template <typename T> auto operator>>(T& value) -> BinaryReader&
		{
			value = read<T>();
			return *this;
		}

		auto operator>>(std::string& str) -> BinaryReader&
		{
			str = readString();
			return *this;
		}

		template <typename T> auto operator>>(std::vector<T>& vec) -> BinaryReader&
		{
			vec = readVector<T>();
			return *this;
		}
	};
}