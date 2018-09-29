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

JSONArray::JSONArray() : JSONAtom(JSON_TYPE_ARRAY) { }

JSONArray::JSONArray(JSONArray &array) : JSONAtom{}
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
					throw JSONArrayError(JSON_ARRAY_BAD_ATOM);
			}
		}(*atom));
	}
}

void array_t::add(jsonAtomPtr_t &&value)
	{ children.emplace_back(std::move(value)); }

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

JSONAtom &JSONArray::operator [](const size_t key) const
{
	if (key >= children.size())
		throw JSONArrayError(JSON_ARRAY_OOB);
	return *children[key];
}

size_t JSONArray::size() const
{
	return children.size();
}

JSONArray::iterator JSONArray::begin() const
{
	return children.begin();
}

JSONArray::iterator JSONArray::end() const
{
	return children.end();
}

void JSONArray::add(jsonAtomPtr_t &&value)
	{ arr->add(std::move(value)); }
void JSONArray::add(JSONAtom *value)
	{ arr->add(jsonAtomPtr_t{value}); }
