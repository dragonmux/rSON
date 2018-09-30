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

#include <algorithm>
#include "internal.h"
#include "String.hxx"

JSONArray::JSONArray() : JSONAtom(JSON_TYPE_ARRAY), arr{makeManaged<array_t>()} { }

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

void array_t::del(const size_t key)
{
	if (key >= children.size())
		throw JSONArrayError(JSON_ARRAY_OOB);
	children.erase(children.begin() + key);
}

void array_t::del(const JSONAtom &value)
{
	const auto &atom = std::find_if(children.begin(), children.end(),
		[&](const jsonAtomPtr_t &atom) -> bool { return atom.get() == &value; });
	children.erase(atom);
}

JSONAtom &array_t::operator [](const size_t key) const
{
	if (key >= children.size())
		throw JSONArrayError(JSON_ARRAY_OOB);
	return *children[key];
}

const JSONAtom *array_t::last() const noexcept
	{ return children.empty() ? nullptr : children.back().get(); }

void JSONArray::add(jsonAtomPtr_t &&value)
	{ arr->add(std::move(value)); }
void JSONArray::add(JSONAtom *value)
	{ arr->add(jsonAtomPtr_t{value}); }
void JSONArray::del(const size_t key) { arr->del(key); }
void JSONArray::del(const JSONAtom *value) { arr->del(*value); }
void JSONArray::del(const JSONAtom &value) { arr->del(value); }
JSONAtom &JSONArray::operator [](const size_t key) const { return (*arr)[key]; }
size_t JSONArray::size() const { return arr->size(); }
const jsonAtomPtr_t *JSONArray::begin() const { return &*arr->begin(); }
const jsonAtomPtr_t *JSONArray::end() const { return &*arr->end(); }
