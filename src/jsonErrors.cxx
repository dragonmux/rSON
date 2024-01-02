// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2014,2017-2018,2020-2021,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include <exception>

#include "internal/types.hxx"
#include "internal/string.hxx"

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

const char *JSONTypeError::typeToString(JSONAtomType type) const noexcept
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
		default:
			break;
	}
	return "unknown";
}

const char *JSONObjectError::error() const noexcept
{
	if (objectError == JSON_OBJECT_BAD_KEY)
		return "Object key does not exist";
	return "Invalid unknown error type for object error";
}

const char *JSONArrayError::error() const noexcept
{
	switch (arrayError)
	{
		case JSON_ARRAY_OOB:
			return "Array index out of bounds";
		case JSON_ARRAY_BAD_ATOM:
			return "Array contains a bad item";
		default:
			break;
	}
	return "Invalid unknown error type for array error";
}
