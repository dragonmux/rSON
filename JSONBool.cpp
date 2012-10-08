#include "internal.h"

JSONBool::JSONBool(bool boolValue) : JSONAtom(JSON_TYPE_BOOL), value(boolValue)
{
}

JSONBool::~JSONBool()
{
}

JSONBool::operator bool() const
{
	return value;
}
