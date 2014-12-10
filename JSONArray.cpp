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

JSONArray::JSONArray() : JSONAtom(JSON_TYPE_ARRAY)
{
}

JSONArray::JSONArray(JSONArray &object) : JSONAtom(JSON_TYPE_ARRAY)
{
	for (size_t i = 0; i < object.children.size(); i++)
	{
		JSONAtom *value, *child = object.children[i];
		switch (child->getType())
		{
			case JSON_TYPE_NULL:
				value = new JSONNull();
				break;
			case JSON_TYPE_BOOL:
				value = new JSONBool(*((JSONBool *)child));
				break;
			case JSON_TYPE_INT:
				value = new JSONInt(*((JSONInt *)child));
				break;
			case JSON_TYPE_FLOAT:
				value = new JSONFloat(*((JSONFloat *)child));
				break;
			case JSON_TYPE_STRING:
				value = new JSONString(strNewDup(*((JSONString *)child)));
				break;
			case JSON_TYPE_OBJECT:
				value = new JSONObject(*((JSONObject *)child));
				break;
			case JSON_TYPE_ARRAY:
				value = new JSONArray(*((JSONArray *)child));
				break;
			default:
				throw JSONObjectError(JSON_OBJECT_BAD_KEY);
		}
		children.push_back(value);
	}
}

JSONArray::~JSONArray()
{
	if (this == NULL)
		return;
	for (size_t i = 0; i < children.size(); i++)
		delete children[i];
	children.clear();
}

void JSONArray::add(JSONAtom *value)
{
	children.push_back(value);
}

void JSONArray::del(size_t key)
{
	childTypeIter i;
	if (key >= children.size())
		throw JSONArrayError(JSON_ARRAY_OOB);

	i = children.begin() + key;
	delete *i;
	children.erase(i);
}

void JSONArray::del(JSONAtom *value)
{
	childTypeIter i;
	for (i = children.begin(); i != children.end(); i++)
	{
		if (*i == value)
		{
			delete value;
			children.erase(i);
			break;
		}
	}
}

JSONAtom *JSONArray::operator [](size_t key)
{
	if (key >= children.size())
		throw JSONArrayError(JSON_ARRAY_OOB);
	return children[key];
}

size_t JSONArray::size()
{
	return children.size();
}
