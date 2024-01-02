// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "internal/types.hxx"

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
