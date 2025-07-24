#include <utility>

namespace utils
{
	class BinaryWriter;
	class BinaryReader;

	class SerializableObject
	{
	public:
		virtual ~SerializableObject() = default;
		virtual void serialize(BinaryWriter& writer) const
		{
		}
		virtual void deserialize(BinaryReader& reader)
		{
		}
	};

	template <typename T> struct HasWriteMethod
	{
		template <typename U>
		static auto test(int)
			-> decltype(std::declval<const U>().serialize(std::declval<BinaryWriter&>()),
						std::true_type{});
		template <typename> static auto test(...) -> std::false_type;

		static constexpr bool Value = decltype(test<T>(0))::value;
	};

	template <typename T> struct HasReadMethod
	{
		template <typename U>
		static auto test(int)
			-> decltype(std::declval<U>().deserialize(std::declval<BinaryReader&>()),
						std::true_type{});
		template <typename> static auto test(...) -> std::false_type;

		static constexpr bool Value = decltype(test<T>(0))::value;
	};

	template <typename T> constexpr bool HasWriteMethodV = HasWriteMethod<T>::Value;

	template <typename T> constexpr bool HasReadMethodV = HasReadMethod<T>::Value;

	enum class Endian : char
	{
		Little,
		Big,
		Native
	};
}