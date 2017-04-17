/*
 * This file is part of rSON
 * Copyright © 2012-2013 Rachel Mant (dx-mon@users.sourceforge.net)
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

JSONObject::JSONObject() : JSONAtom(JSON_TYPE_OBJECT)
{
}

JSONObject::JSONObject(JSONObject &object) : JSONAtom(JSON_TYPE_OBJECT)
{
	for (const auto &child : object.children)
	{
		char *key = strNewDup(child.first);
		JSONAtom *value;
		switch (child.second->getType())
		{
			case JSON_TYPE_NULL:
				value = new JSONNull();
				break;
			case JSON_TYPE_BOOL:
				value = new JSONBool(*child.second);
				break;
			case JSON_TYPE_INT:
				value = new JSONInt(*child.second);
				break;
			case JSON_TYPE_FLOAT:
				value = new JSONFloat(*child.second);
				break;
			case JSON_TYPE_STRING:
				value = new JSONString(strNewDup(*child.second));
				break;
			case JSON_TYPE_OBJECT:
				value = new JSONObject(*child.second);
				break;
			case JSON_TYPE_ARRAY:
				value = new JSONArray(*child.second);
				break;
			default:
				throw JSONObjectError(JSON_OBJECT_BAD_KEY);
		}
		children[key] = value;
	}
}

JSONObject::~JSONObject()
{
	for (auto &child : children)
	{
		delete [] child.first;
		delete child.second;
	}
	mapKeys.clear();
	children.clear();
}

void JSONObject::add(char *key, JSONAtom *value)
{
	atomMapIter node = children.find(key);
	if (node != children.end())
		return;
	children[key] = value;
	mapKeys.push_back((const char *)key);
}

void JSONObject::del(const char *key)
{
	if (!key)
		return;
	atomMapIter node = children.find((char *)key);
	if (node != children.end())
	{
		for (keyTypeIter i = mapKeys.begin(); i != mapKeys.end(); i++)
		{
			if (strcmp(key, *i) == 0)
			{
				mapKeys.erase(i);
				break;
			}
		}
		delete [] node->first;
		delete node->second;
		children.erase(node);
	}
}

JSONAtom &JSONObject::operator [](const char *const key) const
{
	atomMapConstIter node = children.find((char *)key);
	if (node == children.end())
		throw JSONObjectError(JSON_OBJECT_BAD_KEY);
	return *node->second;
}

size_t JSONObject::size() const
{
	return children.size();
}

const std::vector<const char *> &JSONObject::keys() const
{
	return mapKeys;
}

bool JSONObject::exists(const char *key) const
{
	atomMapConstIter node = children.find((char *)key);
	return node != children.end();
}
