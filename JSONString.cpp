#include "internal.h"

JSONString::JSONString(char *strValue) : JSONAtom(JSON_TYPE_STRING), value(strValue)
{
}

JSONString::~JSONString()
{
	delete value;
}

JSONString::operator const char *() const
{
	return value;
}
