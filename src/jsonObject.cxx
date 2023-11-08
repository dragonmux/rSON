/*
 * This file is part of rSON
 * Copyright © 2012-2020 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <algorithm>
#include <substrate/utility>
#include "internal/types.hxx"
#include "internal/string.hxx"

#if !defined(_MSC_VER) || _MSC_VER >= 1928
JSONObject::JSONObject() : JSONAtom{JSON_TYPE_OBJECT}, obj{makeOpaque<object_t>()} { }
#else
JSONObject::JSONObject() : JSONAtom{JSON_TYPE_OBJECT}, obj{} { }
#endif

JSONObject::JSONObject(JSONObject &object) : JSONObject{}
	{ obj->clone(*object.obj); }

void object_t::clone(const object_t &object)
{
	for (const auto &atom : object)
	{
		add(atom.first.get(), [](const JSONAtom &value) -> std::unique_ptr<JSONAtom>
		{
			switch (value.getType())
			{
				case JSON_TYPE_NULL:
					return substrate::make_unique<JSONNull>();
				case JSON_TYPE_BOOL:
					return substrate::make_unique<JSONBool>(value);
				case JSON_TYPE_INT:
					return substrate::make_unique<JSONInt>(value);
				case JSON_TYPE_FLOAT:
					return substrate::make_unique<JSONFloat>(value);
				case JSON_TYPE_STRING:
					return substrate::make_unique<JSONString>(value);
				case JSON_TYPE_OBJECT:
					return substrate::make_unique<JSONObject>(value);
				case JSON_TYPE_ARRAY:
					return substrate::make_unique<JSONArray>(value);
				default:
					throw JSONObjectError(JSON_OBJECT_BAD_KEY);
			}
		}(*atom.second));
	}
}

JSONAtom *object_t::add(const char *const keyStr, std::unique_ptr<JSONAtom> &&value)
{
	if (children.find(keyStr) != children.end())
		return nullptr;
	auto key{stringDup(keyStr)};
	mapKeys.push_back(key.get());
	const auto result{children.emplace(std::move(key), std::move(value))};
	if (result.second)
		return result.first->second.get();
	return nullptr;
}

void object_t::del(const char *const key)
{
	if (!key)
		return;
	const auto &atom = children.find(key);
	if (atom != children.end())
	{
		const auto &atomKey = std::find_if(mapKeys.begin(), mapKeys.end(),
			[&](const char *const atom) -> bool { return strcmp(key, atom) == 0; });
		mapKeys.erase(atomKey);
		children.erase(atom);
	}
}

JSONAtom &object_t::operator [](const char *const key) const
{
	const auto &node = children.find(key);
	if (node == children.end())
		throw JSONObjectError(JSON_OBJECT_BAD_KEY);
	return *node->second;
}

bool object_t::exists(const char *const key) const noexcept
	{ return children.find(key) != children.end(); }

void JSONObject::add(const char *const key, jsonAtomPtr_t &&value)
	{ obj->add(key, std::move(value)); }
void JSONObject::add(const char *const key, JSONAtom *value)
	{ obj->add(key, jsonAtomPtr_t{value}); }
void JSONObject::del(const char *const key) { obj->del(key); }
JSONAtom &JSONObject::operator [](const char *const key) const { return (*obj)[key]; }
const std::vector<const char *> &JSONObject::keys() const { return obj->keys(); }
bool JSONObject::exists(const char *const key) const { return obj->exists(key); }
size_t JSONObject::size() const { return obj->size(); }

bool JSONObject::add(const char *const key, std::nullptr_t)
	{ return obj->add(key, std::make_unique<JSONNull>()); }
bool JSONObject::add(const char *const key, const bool value)
	{ return obj->add(key, std::make_unique<JSONBool>(value)); }
bool JSONObject::add(const char *const key, const int64_t value)
	{ return obj->add(key, std::make_unique<JSONInt>(value)); }
bool JSONObject::add(const char *const key, const double value)
	{ return obj->add(key, std::make_unique<JSONFloat>(value)); }
bool JSONObject::add(const char *const key, const std::string &value)
	{ return obj->add(key, std::make_unique<JSONString>(value)); }
bool JSONObject::add(const char *const key, std::string &&value)
	{ return obj->add(key, std::make_unique<JSONString>(std::move(value))); }
bool JSONObject::add(const char *const key, const std::string_view &value)
	{ return obj->add(key, std::make_unique<JSONString>(value)); }
