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
#include "Memory.h"
#include <string.h>
#include <math.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

// Temporary terminal of stream store to make the compiler happy
void JSONAtom::store(stream_t &) const { }
void JSONAtom::store(char *str)
{
	memoryStream_t memory(str, length());
	store(memory);
}

size_t JSONNull::length() const { return 4; }
void JSONNull::store(stream_t &stream) const
	{ stream.write("null", 4); }

size_t JSONInt::length() const { return formatLen("%d", value); }

void JSONInt::store(char *str)
{
	snprintf(str, length() + 1, "%d", value);
}

size_t JSONFloat::length() const
{
	return formatLen("%f", value);
}

void JSONFloat::store(char *str)
{
	snprintf(str, length() + 1, "%f", value);
}

size_t JSONString::length() const
	{ return strlen(value) + 2; }

void JSONString::store(stream_t &stream) const
{
	stream.write('"');
	stream.write(value, strlen(value));
	stream.write('"');
}

size_t JSONBool::length() const
	{ return value ? 4 : 5; }

void JSONBool::store(stream_t &stream) const
{
	if (value)
		stream.write("true", 4);
	else
		stream.write("false", 5);
}

size_t JSONObject::length() const
{
	size_t nChildren, len = 2;
	for (const auto &child : children)
	{
		len += strlen(child.first) + 2;
		len += child.second->length() + 2;
	}
	nChildren = size();
	if (nChildren > 0)
		len += (size() - 1) * 2;
	return len;
}

void JSONObject::store(stream_t &stream) const
{
	const size_t nodes = size();
	size_t j = 0;

	stream.write('{');
	for (const auto &child : children)
	{
		stream.write('"');
		stream.write(child.first, strlen(child.first));
		stream.write("\": ", 3);
		child.second->store(stream);
		if (++j < nodes)
			stream.write(", ", 2);
	}
	stream.write('}');
}

size_t JSONArray::length() const
{
	size_t i, len = 2;
	for (i = 0; i < size(); i++)
		len += children[i]->length();
	if (size() > 0)
		len += (size() - 1) * 2;
	return len;
}

void JSONArray::store(stream_t &stream) const
{
	const JSONAtom *const last = children.empty() ? nullptr : children.back();

	stream.write('[');
	for (const auto &child : children)
	{
		child->store(stream);
		if (child != last)
			stream.write(", ", 2);
	}
	stream.write(']');
}

char *rSON::writeJSON(JSONAtom *atom)
{
	size_t strLength;
	char *str;

	if (atom == NULL)
		return NULL;

	strLength = atom->length();
	str = new char[strLength + 1]();
	atom->store(str);
	str[strLength] = 0;

	return str;
}

void rSON::freeString(char **str)
{
	if (str == NULL)
		return;
	delete [] *str;
	*str = NULL;
}
