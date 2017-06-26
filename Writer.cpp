/*
 * This file is part of rSON
 * Copyright © 2012-2017 Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * rSON is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rSON is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "internal.h"
#include "String.h"
#include <type_traits>
#include <utility>
#include <string.h>
#include <math.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

template<typename> struct isBoolean_ : public std::false_type { };
template<> struct isBoolean_<bool> : public std::true_type { };
template<typename T> struct isBoolean : public std::integral_constant<bool,
	isBoolean_<typename std::remove_cv<T>::type>::value> { };

// Type aliases for some type_traits utilities to make them easier to type and read below
template<bool B, typename T = void> using enableIf = typename std::enable_if<B, T>::type;
template<typename A, typename B> using isSame = std::is_same<A, B>;
template<typename A> using isIntegral = std::is_integral<A>;
template<typename A> using isUnsigned = std::is_unsigned<A>;
template<typename A> using isSigned = std::is_signed<A>;
template<typename A> using makeUnsigned = typename std::make_unsigned<A>::type;

size_t JSONNull::length() const { return 4; }
void JSONNull::store(stream_t &stream) const
	{ stream.write("null", 4); }

template<typename int_t, typename valueType_t> struct fromInt_t
{
private:
	using uint_t = makeUnsigned<int_t>;
	const valueType_t &_value;

	uint8_t calcDigits(const uint_t number) const noexcept
	{
		if (number < 10)
			return 1;
		return 1 + calcDigits(number / 10);
	}

	uint8_t digits(const int_t number) const noexcept
	{
		if (isSigned<int_t>::value && number < 0)
			return 1 + calcDigits(-number);
		return calcDigits(number);
	}

	[[gnu::noinline]] uint_t process(const uint_t number, stream_t &stream, const uint8_t digits, const size_t index) const noexcept
	{
		const uint_t num = number < 10 ? 0 : process(number / 10, stream, digits, index + 1) * 10;
		stream.write(char(number - num + '0'));
		return number;
	}

	void process(const uint_t number, stream_t &stream, const size_t index = 0) const noexcept
		{ process(number, stream, digits(_value) - 1, index); }
	template<typename T = int_t> enableIf<isSame<T, int_t>::value && isIntegral<T>::value && !isBoolean<T>::value && isUnsigned<T>::value>
		format(stream_t &stream) const noexcept { process(_value, stream); }
	template<typename T = int_t> [[gnu::noinline]] enableIf<isSame<T, int_t>::value && isIntegral<T>::value && !isBoolean<T>::value && isSigned<T>::value>
		format(stream_t &stream) const noexcept
	{
		int_t number = _value;
		if (number < 0)
		{
			stream.write('-');
			process(uint_t(-number), stream, 1);
		}
		else
			process(uint_t(number), stream);
	}

public:
	constexpr fromInt_t(const valueType_t &value) noexcept : _value(value) { }
	uint8_t length() const noexcept { return digits(_value); }
	bool convert(stream_t &stream) const noexcept { format(stream); return stream.atEOF(); }
};

size_t JSONInt::length() const { return fromInt_t<int32_t, int32_t>(value).length(); }

void JSONInt::store(stream_t &stream) const
	{ fromInt_t<int32_t, int32_t>(value).convert(stream); }

size_t JSONFloat::length() const { return formatLen("%f", value); }

// This is better.. but %f is wrong and produces very much the wrong result.
void JSONFloat::store(stream_t &stream) const
{
	const auto string = formatString("%f", value);
	stream.write(string.get(), strlen(string.get()));
}

size_t JSONString::length() const
	{ return strlen(value) + 2; }

void JSONString::store(stream_t &stream) const
{
	stream.write('"');
	stream.write(value, strlen(value));
	stream.write('"');
}

size_t JSONBool::length() const
	{ return value ? 4 : 5; }

void JSONBool::store(stream_t &stream) const
{
	if (value)
		stream.write("true", 4);
	else
		stream.write("false", 5);
}

size_t JSONObject::length() const
{
	size_t nChildren, len = 2;
	for (const auto &child : children)
	{
		len += strlen(child.first) + 2;
		len += child.second->length() + 2;
	}
	nChildren = size();
	if (nChildren > 0)
		len += (size() - 1) * 2;
	return len;
}

void JSONObject::store(stream_t &stream) const
{
	const size_t nodes = size();
	size_t j = 0;

	stream.write('{');
	for (const auto &child : children)
	{
		stream.write('"');
		stream.write(child.first, strlen(child.first));
		stream.write("\": ", 3);
		child.second->store(stream);
		if (++j < nodes)
			stream.write(", ", 2);
	}
	stream.write('}');
}

size_t JSONArray::length() const
{
	size_t i, len = 2;
	for (i = 0; i < size(); i++)
		len += children[i]->length();
	if (size() > 0)
		len += (size() - 1) * 2;
	return len;
}

void JSONArray::store(stream_t &stream) const
{
	const JSONAtom *const last = children.empty() ? nullptr : children.back();

	stream.write('[');
	for (const auto &child : children)
	{
		child->store(stream);
		if (child != last)
			stream.write(", ", 2);
	}
	stream.write(']');
}

void rSON::writeJSON(const JSONAtom *const atom, stream_t &stream)
{
	if (atom == nullptr)
		return;
	atom->store(stream);
	stream.sync();
}

char *rSON::writeJSON(JSONAtom *atom)
{
	size_t strLength;
	char *str;

	if (atom == nullptr)
		return nullptr;

	strLength = atom->length();
	str = new char[strLength + 1]();
	memoryStream_t stream(str, strLength);
	atom->store(stream);
	str[strLength] = 0;

	return str;
}

void rSON::freeString(char **str)
{
	if (str == nullptr)
		return;
	delete [] *str;
	*str = nullptr;
}
