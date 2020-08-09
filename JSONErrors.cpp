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

#include <exception>
#include <malloc.h>

#include "internal.h"
#include "String.hxx"

const char *JSONParserError::error() const noexcept
{
	switch (parserError)
	{
		case JSON_PARSER_EOF:
			return "The JSON parser has reached the end of the data but failed to terminate";
		case JSON_PARSER_BAD_JSON:
			return "The JSON parser has determined it was fed with bad JSON";
		case JSON_PARSER_BAD_FILE:
			return "The JSON parser could not read the file it was asked to parse";
		default:
			break;
	}
	return "Invalid unknown error type for parser error";
}

JSONTypeError::JSONTypeError(JSONAtomType actual, JSONAtomType expected)
{
	errorStr = formatString("Expecting %s, found %s", typeToString(expected), typeToString(actual));
}

const char *JSONTypeError::typeToString(JSONAtomType type) const
{
	switch (type)
	{
		case JSON_TYPE_NULL:
			return "null";
		case JSON_TYPE_BOOL:
			return "bool";
		case JSON_TYPE_INT:
			return "int";
		case JSON_TYPE_FLOAT:
			return "float";
		case JSON_TYPE_STRING:
			return "string";
		case JSON_TYPE_OBJECT:
			return "object";
		case JSON_TYPE_ARRAY:
			return "array";
	}
	throw std::exception();
}

const char *JSONTypeError::error() const { return errorStr.get(); }
JSONObjectError::JSONObjectError(JSONObjectErrorType errorType) : objectError(errorType) { }

const char *JSONObjectError::error() const
{
	switch (objectError)
	{
		case JSON_OBJECT_BAD_KEY:
			return "Object key does not exist";
		default:
			throw std::exception();
	}
}

JSONArrayError::JSONArrayError(JSONArrayErrorType errorType) : arrayError(errorType) { }

const char *JSONArrayError::error() const
{
	switch (arrayError)
	{
		case JSON_ARRAY_OOB:
			return "Array index out of bounds";
		case JSON_ARRAY_BAD_ATOM:
			return "Array contains a bad item";
		default:
			throw std::exception();
	}
}
