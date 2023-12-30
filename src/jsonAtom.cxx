/*
 * This file is part of rSON
 * Copyright © 2012-2016 Rachel Mant (dx-mon@users.sourceforge.net)
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
