#pragma once
#include "SerializableObject.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace utils
{
	class BinaryWriter
	{
	private:
		std::ostream* _stream;
		bool _ownsStream;
		Endian _endian;

		template <typename T> void _writeValue(const T& value)
		{
			static_assert(std::is_trivially_copyable_v<T>,
						  "Type must be trivially copyable");

			if constexpr (sizeof(T) == 1)
			{
				_stream->write(reinterpret_cast<const char*>(&value), sizeof(T));
			}
			else
			{
				T converted_value = value;
				if (_needsEndianSwap())
				{
					converted_value = _swapEndian(value);
				}
				_stream->write(reinterpret_cast<const char*>(&converted_value),
							   sizeof(T));
			}

			if (!_stream->good())
			{
				throw std::runtime_error("Write operation failed");
			}
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
		// Constructors
		explicit BinaryWriter(std::ostream& stream, Endian endian = Endian::Native)
			: _stream(&stream), _ownsStream(false), _endian(endian)
		{
		}

		explicit BinaryWriter(const std::string& filename, Endian endian = Endian::Native)
			: _ownsStream(true), _endian(endian)
		{
			auto* file_stream = new std::ofstream(filename, std::ios::binary);
			if (!file_stream->is_open())
			{
				delete file_stream;
				throw std::runtime_error("Failed to open file: " + filename);
			}
			_stream = file_stream;
		}

		~BinaryWriter()
		{
			if (_ownsStream)
			{
				delete _stream;
			}
		}

		BinaryWriter(const BinaryWriter&) = delete;
		auto operator=(const BinaryWriter&) -> BinaryWriter& = delete;
		BinaryWriter(BinaryWriter&&) = default;
		auto operator=(BinaryWriter&&) -> BinaryWriter& = default;

		template <typename T> auto write(const T& value) -> BinaryWriter&
		{
			if constexpr (std::is_arithmetic_v<T>)
			{
				_writeValue(value);
			}
			else if constexpr (std::is_same_v<std::string, T>)
			{
				writeString(value);
			}
			else if constexpr (HasWriteMethodV<T>)
			{
				value.write(*this);
			}
			else
			{
				static_assert(
					std::is_arithmetic_v<T> || HasWriteMethodV<T>,
					"Type must be arithmetic or have a write(BinaryWriter&) method");
			}
			return *this;
		}

		auto writeString(const std::string& str, bool nullTerminated = false) -> BinaryWriter&
		{
			_writeValue(static_cast<uint32_t>(str.length()));

			_stream->write(str.data(), (long)str.length());

			if (nullTerminated)
			{
				_writeValue(static_cast<uint8_t>(0));
			}

			if (!_stream->good())
			{
				throw std::runtime_error("String write operation failed");
			}
			return *this;
		}

		auto writeString(const char* str, bool nullTerminated = true) -> BinaryWriter&
		{
			size_t len = std::strlen(str);
			_writeValue(static_cast<uint32_t>(len));
			_stream->write(str, (long)len);

			if (nullTerminated)
			{
				_writeValue(static_cast<uint8_t>(0));
			}

			if (!_stream->good())
			{
				throw std::runtime_error("C-string write operation failed");
			}
			return *this;
		}

		template <typename T> auto writeVector(const std::vector<T>& vec) -> BinaryWriter&
		{
			_writeValue(static_cast<uint32_t>(vec.size()));

			for (const auto& item : vec)
			{
				if constexpr (std::is_arithmetic_v<T>)
				{
					_writeValue(item);
				}
				else if constexpr (HasWriteMethodV<T>)
				{
					item.write(*this);
				}
				else
				{
					write(item);
				}
			}
			return *this;
		}

		auto writeBuffer(const void* data, size_t size) -> BinaryWriter&
		{
			_stream->write(static_cast<const char*>(data), (long)size);
			if (!_stream->good())
			{
				throw std::runtime_error("Buffer write operation failed");
			}
			return *this;
		}

		// Padding and alignment
		auto writePadding(size_t bytes, uint8_t padValue = 0) -> BinaryWriter&
		{
			for (size_t i = 0; i < bytes; ++i)
			{
				_writeValue(padValue);
			}
			return *this;
		}

		auto alignTo(size_t boundary) -> BinaryWriter&
		{
			size_t pos = _stream->tellp();
			size_t padding = (boundary - (pos % boundary)) % boundary;
			if (padding > 0)
			{
				writePadding(padding);
			}
			return *this;
		}

		[[nodiscard]] auto tell() const -> size_t
		{
			return _stream->tellp();
		}

		auto seek(size_t pos) -> BinaryWriter&
		{
			_stream->seekp((long)pos);
			return *this;
		}

		template <typename T> auto operator<<(const T& value) -> BinaryWriter&
		{
			return write(value);
		}

		auto operator<<(const std::string& str) -> BinaryWriter&
		{
			return writeString(str);
		}

		template <typename T> auto operator<<(const std::vector<T>& vec) -> BinaryWriter&
		{
			return write_vector(vec);
		}
	};
}