// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013 2015-2019 2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "internal/types.hxx"

void *JSONAtom::asNull() const
{
	if (!typeIs(JSON_TYPE_NULL))
		throw JSONTypeError(type, JSON_TYPE_NULL);
	return nullptr;
}

bool JSONAtom::asBool() const
{
	if (!typeIs(JSON_TYPE_BOOL))
		throw JSONTypeError(type, JSON_TYPE_BOOL);
	return *((JSONBool *)this);
}

JSONAtom::operator int64_t() const
{
	if (!typeIs(JSON_TYPE_INT))
		throw JSONTypeError(type, JSON_TYPE_INT);
	return *((JSONInt *)this);
}

JSONAtom::operator double() const
{
	if (!typeIs(JSON_TYPE_FLOAT))
		throw JSONTypeError(type, JSON_TYPE_FLOAT);
	return *((JSONFloat *)this);
}

JSONString &JSONAtom::asStringRef() const
{
	if (!typeIs(JSON_TYPE_STRING))
		throw JSONTypeError(type, JSON_TYPE_STRING);
	return *((JSONString *)this);
}

JSONAtom::operator JSONString &() const
	{ return asStringRef(); }
JSONAtom::operator const char *() const
	{ return asStringRef(); }
JSONAtom::operator const std::string &() const
	{ return asStringRef(); }

JSONObject *JSONAtom::asObject() const
{
	if (!typeIs(JSON_TYPE_OBJECT))
		throw JSONTypeError(type, JSON_TYPE_OBJECT);
	return (JSONObject *)this;
}

JSONAtom::operator JSONObject &() const
	{ return *asObject(); }

JSONArray *JSONAtom::asArray() const
{
	if (!typeIs(JSON_TYPE_ARRAY))
		throw JSONTypeError(type, JSON_TYPE_ARRAY);
	return (JSONArray *)this;
}

JSONAtom::operator JSONArray &() const
	{ return *asArray(); }
JSONAtom &JSONAtom::operator [](const char *const key) const
	{ return asObjectRef()[std::string_view{key}]; }
JSONAtom &JSONAtom::operator [](const std::string &key) const
	{ return asObjectRef()[std::string_view{key}]; }
JSONAtom &JSONAtom::operator [](const std::string_view key) const
	{ return asObjectRef()[key]; }
JSONAtom &JSONAtom::operator [](const size_t key) const
	{ return asArrayRef()[key]; }

bool JSONAtom::add(std::string &&key, std::nullptr_t)
	{ return asObject()->add(std::move(key), nullptr); }
bool JSONAtom::add(std::string &&key, const bool value)
	{ return asObject()->add(std::move(key), value); }
bool JSONAtom::add(std::string &&key, const int64_t value)
	{ return asObject()->add(std::move(key), value); }
bool JSONAtom::add(std::string &&key, const double value)
	{ return asObject()->add(std::move(key), value); }
bool JSONAtom::add(std::string &&key, const std::string &value)
	{ return asObject()->add(std::move(key), value); }
bool JSONAtom::add(std::string &&key, std::string &&value)
	{ return asObject()->add(std::move(key), std::move(value)); }
bool JSONAtom::add(std::string &&key, const std::string_view &value)
	{ return asObject()->add(std::move(key), value); }
JSONArray *JSONAtom::addArray(std::string &&key)
	{ return asObject()->addArray(std::move(key)); }
JSONObject *JSONAtom::addObject(std::string &&key)
	{ return asObject()->addObject(std::move(key)); }

bool JSONAtom::add(const std::string_view &key, std::nullptr_t)
	{ return asObject()->add(key, nullptr); }
bool JSONAtom::add(const std::string_view &key, const bool value)
	{ return asObject()->add(key, value); }
bool JSONAtom::add(const std::string_view &key, const int64_t value)
	{ return asObject()->add(key, value); }
bool JSONAtom::add(const std::string_view &key, const double value)
	{ return asObject()->add(key, value); }
bool JSONAtom::add(const std::string_view &key, const std::string &value)
	{ return asObject()->add(key, value); }
bool JSONAtom::add(const std::string_view &key, std::string &&value)
	{ return asObject()->add(key, std::move(value)); }
bool JSONAtom::add(const std::string_view &key, const std::string_view &value)
	{ return asObject()->add(key, value); }
JSONArray *JSONAtom::addArray(const std::string_view &key)
	{ return asObject()->addArray(key); }
JSONObject *JSONAtom::addObject(const std::string_view &key)
	{ return asObject()->addObject(key); }

void JSONAtom::add(std::nullptr_t)
	{ asArray()->add(nullptr); }
void JSONAtom::add(const bool value)
	{ asArray()->add(value); }
void JSONAtom::add(const int64_t value)
	{ asArray()->add(value); }
void JSONAtom::add(const double value)
	{ asArray()->add(value); }
void JSONAtom::add(const std::string &value)
	{ asArray()->add(value); }
void JSONAtom::add(std::string &&value)
	{ asArray()->add(std::move(value)); }
void JSONAtom::add(const std::string_view &value)
	{ asArray()->add(value); }
JSONArray &JSONAtom::addArray()
	{ return asArray()-> addArray(); }
JSONObject &JSONAtom::addObject()
	{ return asArray()-> addObject(); }
