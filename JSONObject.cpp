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

JSONObject::JSONObject() : JSONAtom(JSON_TYPE_OBJECT)
{
}

JSONObject::~JSONObject()
{
	for (atomMapIter i = children.begin(); i != children.end(); i++)
	{
		delete i->first;
		delete i->second;
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

JSONAtom *JSONObject::operator [](const char *key)
{
	atomMapIter node = children.find((char *)key);
	if (node == children.end())
		throw JSONObjectError(JSON_OBJECT_BAD_KEY);
	return node->second;
}

size_t JSONObject::size()
{
	return children.size();
}

std::vector<const char *> &JSONObject::keys()
{
	return mapKeys;
}
