// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2020,2023-2024 Rachel Mant <git@dragonmux.network>
// SPDX-FileCopyrightText: 2021 Amyspark <amy@amyspark.me>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Modified by Amyspark <amy@amyspark.me>

#include <algorithm>
#include "internal/types.hxx"
#include "internal/string.hxx"

#if !defined(_MSC_VER) || _MSC_VER >= 1928
JSONArray::JSONArray() : JSONAtom{JSON_TYPE_ARRAY}, arr{makeOpaque<array_t>()} { }
#else
JSONArray::JSONArray() : JSONAtom{JSON_TYPE_ARRAY}, arr{} {}
#endif

JSONArray::JSONArray(JSONArray &array) : JSONArray{}
	{ arr->clone(*array.arr); }

void array_t::clone(const array_t &array)
{
	for (const auto &atom : array)
	{
		add([](const JSONAtom &value) -> std::unique_ptr<JSONAtom>
		{
			switch (value.getType())
			{
				case JSON_TYPE_NULL:
					return std::make_unique<JSONNull>();
				case JSON_TYPE_BOOL:
					return std::make_unique<JSONBool>(value.asBool());
				case JSON_TYPE_INT:
					return std::make_unique<JSONInt>(value);
				case JSON_TYPE_FLOAT:
					return std::make_unique<JSONFloat>(value);
				case JSON_TYPE_STRING:
					return std::make_unique<JSONString>(value.asString());
				case JSON_TYPE_OBJECT:
					return std::make_unique<JSONObject>(value);
				case JSON_TYPE_ARRAY:
					return std::make_unique<JSONArray>(value);
				default:
					throw JSONArrayError(JSON_ARRAY_BAD_ATOM);
			}
		}(*atom));
	}
}

JSONAtom &array_t::add(std::unique_ptr<JSONAtom> &&value)
	{ return *children.emplace_back(std::move(value)); }

void array_t::del(const size_t key)
{
	if (key >= children.size())
		throw JSONArrayError{JSON_ARRAY_OOB};
	children.erase(children.begin() + key);
}

void array_t::del(const JSONAtom &value)
{
	const auto &atom = std::find_if(children.begin(), children.end(),
		[&](const std::unique_ptr<JSONAtom> &atom) -> bool { return atom.get() == &value; });
	children.erase(atom);
}

JSONAtom &array_t::operator [](const size_t key) const
{
	if (key >= children.size())
		throw JSONArrayError{JSON_ARRAY_OOB};
	return *children[key];
}

const JSONAtom *array_t::last() const noexcept
	{ return children.empty() ? nullptr : children.back().get(); }

void JSONArray::add(std::unique_ptr<JSONAtom> &&value)
	{ arr->add(std::move(value)); }
void JSONArray::add(JSONAtom *value)
	{ arr->add(std::unique_ptr<JSONAtom>{value}); }
void JSONArray::del(const size_t key) { arr->del(key); }

void JSONArray::del(const JSONAtom *value)
{
	if (value)
		arr->del(*value);
}

void JSONArray::del(const JSONAtom &value) { arr->del(value); }
JSONAtom &JSONArray::operator [](const size_t key) const { return (*arr)[key]; }
size_t JSONArray::size() const { return arr->size(); }

JSONArray::iterator JSONArray::begin() noexcept
{
	// This must be a lambda otherwise constexpr evaluation of the pointer check fails
	return [&]()
	{
		auto iter{arr->begin()};
		if constexpr (std::is_pointer_v<decltype(iter)>)
			return iter;
		else
			return iter.operator ->();
	}();
}

JSONArray::iterator JSONArray::begin() const noexcept
{
	// This must be a lambda otherwise constexpr evaluation of the pointer check fails
	return [&]()
	{
		const auto iter{arr->begin()};
		if constexpr (std::is_pointer_v<decltype(iter)>)
			return iter;
		else
			return iter.operator ->();
	}();
}

JSONArray::iterator JSONArray::end() noexcept
{
	// This must be a lambda otherwise constexpr evaluation of the pointer check fails
	return [&]()
	{
		auto iter{arr->end()};
		if constexpr (std::is_pointer_v<decltype(iter)>)
			return iter;
		else
			return iter.operator ->();
	}();
}

JSONArray::iterator JSONArray::end() const noexcept
{
	// This must be a lambda otherwise constexpr evaluation of the pointer check fails
	return [&]()
	{
		const auto iter{arr->end()};
		if constexpr (std::is_pointer_v<decltype(iter)>)
			return iter;
		else
			return iter.operator ->();
	}();
}

void JSONArray::add(std::nullptr_t)
	{ arr->add(std::make_unique<JSONNull>()); }
void JSONArray::add(const bool value)
	{ arr->add(std::make_unique<JSONBool>(value)); }
void JSONArray::add(const int64_t value)
	{ arr->add(std::make_unique<JSONInt>(value)); }
void JSONArray::add(const double value)
	{ arr->add(std::make_unique<JSONFloat>(value)); }
void JSONArray::add(const std::string &value)
	{ arr->add(std::make_unique<JSONString>(value)); }
void JSONArray::add(std::string &&value)
	{ arr->add(std::make_unique<JSONString>(std::move(value))); }
void JSONArray::add(const std::string_view &value)
	{ arr->add(std::make_unique<JSONString>(value)); }

JSONArray &JSONArray::addArray()
{
	auto &result{arr->add(std::make_unique<JSONArray>())};
	return static_cast<JSONArray &>(result);
}

JSONObject &JSONArray::addObject()
{
	auto &result{arr->add(std::make_unique<JSONObject>())};
	return static_cast<JSONObject &>(result);
}
