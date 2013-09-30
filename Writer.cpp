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

size_t JSONNull::length()
{
	return 4;
}

void JSONNull::store(char *str)
{
	memcpy(str, "null", 4);
}

size_t JSONInt::length()
{
	return formatLen("%d", value);
}

void JSONInt::store(char *str)
{
	snprintf(str, length() + 1, "%d", value);
}

size_t JSONFloat::length()
{
	return formatLen("%f", value);
}

void JSONFloat::store(char *str)
{
	snprintf(str, length() + 1, "%f", value);
}

size_t JSONString::length()
{
	return strlen(value) + 2;
}

void JSONString::store(char *str)
{
	size_t len = strlen(value);

	str[0] = '"';
	memcpy(str + 1, value, len);
	str[len + 1] = '"';
}

size_t JSONBool::length()
{
	return value ? 4 : 5;
}

void JSONBool::store(char *str)
{
	if (value)
		memcpy(str, "true", 4);
	else
		memcpy(str, "false", 5);
}

size_t JSONObject::length()
{
	atomMapIter child;
	size_t nChildren, len = 2;
	for (child = children.begin(); child != children.end(); child++)
	{
		len += strlen(child->first) + 2;
		len += child->second->length() + 2;
	}
	nChildren = size();
	if (nChildren > 0)
		len += (size() - 1) * 2;
	return len;
}

void JSONObject::store(char *str)
{
	atomMapIter child;
	size_t i = 0, j = 0, nodes = size();

	str[i++] = '{';
	for (child = children.begin(); child != children.end(); child++)
	{
		str[i++] = '"';
		memcpy(str + i, child->first, strlen(child->first));
		i += strlen(child->first);
		str[i++] = '"';
		memcpy(str + i, ": ", 2);
		i += 2;
		child->second->store(str + i);
		i += child->second->length();
		j++;
		if (j < nodes)
		{
			memcpy(str + i, ", ", 2);
			i += 2;
		}
	}
	str[i] = '}';
}

size_t JSONArray::length()
{
	size_t i, len = 2;
	for (i = 0; i < size(); i++)
		len += children[i]->length() + 2;
	if (size() > 0)
		len += (size() - 1) * 2;
	return len;
}

void JSONArray::store(char *str)
{
	size_t i = 0, j = 0, nodes = size() - 1;

	str[i++] = '[';
	for (j = 0; j < size(); j++)
	{
		children[j]->store(str + i);
		i += children[j]->length();
		if (j < nodes)
		{
			memcpy(str + i, ", ", 2);
			i += 2;
		}
	}
	str[i] = ']';
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
