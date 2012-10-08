#include <exception>

#include "internal.h"

JSONTypeError::JSONTypeError(JSONAtomType actualType, JSONAtomType expectedType) : actual(actualType), expected(expectedType)
{
}

JSONTypeError::~JSONTypeError()
{
	delete errorStr;
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
