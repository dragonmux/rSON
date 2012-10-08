#include "internal.h"

JSONFloat::JSONFloat(double floatValue) : JSONAtom(JSON_TYPE_FLOAT), value(floatValue)
{
}

JSONFloat::~JSONFloat()
{
}

JSONFloat::operator double() const
{
	return value;
}
