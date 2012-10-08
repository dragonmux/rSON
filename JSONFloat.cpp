#include "internal.h"

JSONFloat::JSONFloat(double floatValue) : value(floatValue)
{
}

JSONFloat::~JSONFloat()
{
}

JSONFloat::operator double() const
{
	return value;
}
