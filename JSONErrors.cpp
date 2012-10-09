#include <exception>
#include <malloc.h>

#include "internal.h"
#include "String.h"

JSONParserError::JSONParserError(JSONParserErrorType errorType) : parserError(errorType)
{
}

JSONParserError::~JSONParserError()
{
}

const char *JSONParserError::error() const
{
	switch (parserError)
	{
		case JSON_PARSER_EOF:
			return "The JSON parser has reached the end of the data but failed to terminate";
		case JSON_PARSER_BAD_JSON:
			return "The JSON parser has determined it was fed with bad JSON";
		default:
			throw std::exception();
	}
}

JSONTypeError::JSONTypeError(JSONAtomType actual, JSONAtomType expected)
{
	errorStr = formatString("Expecting %s, found %s", typeToString(expected), typeToString(actual));
}

JSONTypeError::~JSONTypeError()
{
	free(errorStr);
}

const char *JSONTypeError::typeToString(JSONAtomType type) const
{
	switch (type)
	{
		case JSON_TYPE_NULL:
			return "Null";
		case JSON_TYPE_BOOL:
			return "Bool";
		case JSON_TYPE_INT:
			return "Int";
		case JSON_TYPE_FLOAT:
			return "Float";
		case JSON_TYPE_STRING:
			return "String";
		case JSON_TYPE_OBJECT:
			return "Object";
		case JSON_TYPE_ARRAY:
			return "Array";
	}
	throw std::exception();
}

const char *JSONTypeError::error() const
{
	return errorStr;
}

JSONObjectError::JSONObjectError(JSONObjectErrorType errorType) : objectError(errorType)
{
}

JSONObjectError::~JSONObjectError()
{
}

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

JSONArrayError::JSONArrayError(JSONArrayErrorType errorType) : arrayError(errorType)
{
}

JSONArrayError::~JSONArrayError()
{
}

const char *JSONArrayError::error() const
{
	switch (arrayError)
	{
		case JSON_ARRAY_OOB:
			return "Array index out of bounds";
		default:
			throw std::exception();
	}
}
