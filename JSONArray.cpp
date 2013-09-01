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

JSONArray::JSONArray() : JSONAtom(JSON_TYPE_ARRAY)
{
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
