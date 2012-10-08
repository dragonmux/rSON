#include "internal.h"

JSONInt::JSONInt(int intValue) : JSONAtom(JSON_TYPE_INT), value(intValue)
{
}

JSONInt::~JSONInt()
{
}

JSONInt::operator int() const
{
	return value;
}
