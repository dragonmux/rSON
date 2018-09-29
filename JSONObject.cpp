/*
 * This file is part of rSON
 * Copyright © 2012-2018 Rachel Mant (dx-mon@users.sourceforge.net)
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

JSONObject::JSONObject() : JSONAtom{JSON_TYPE_OBJECT}, obj{makeManaged<object_t>()} { }

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
					return makeUnique<JSONNull>();
				case JSON_TYPE_BOOL:
					return makeUnique<JSONBool>(value);
				case JSON_TYPE_INT:
					return makeUnique<JSONInt>(value);
				case JSON_TYPE_FLOAT:
					return makeUnique<JSONFloat>(value);
				case JSON_TYPE_STRING:
					return makeUnique<JSONString>(strNewDup(value));
				case JSON_TYPE_OBJECT:
					return makeUnique<JSONObject>(value);
				case JSON_TYPE_ARRAY:
					return makeUnique<JSONArray>(value);
				default:
					throw JSONObjectError(JSON_OBJECT_BAD_KEY);
			}
		}(*atom.second));
	}
}

void object_t::add(const char *const keyStr, std::unique_ptr<JSONAtom> &&value)
{
	if (children.find(keyStr) != children.end())
		return;
	string_t key = stringDup(keyStr);
	mapKeys.push_back(key.get());
	children[std::move(key)] = std::move(value);
}

void object_t::del(const char *const key)
{
	if (!key)
		return;
	const auto &atom = children.find(key);
	if (atom != children.end())
	{
		for (auto i = mapKeys.begin(); i != mapKeys.end(); ++i)
		{
			if (strcmp(key, *i) == 0)
			{
				mapKeys.erase(i);
				break;
			}
		}
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
