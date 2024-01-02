// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2014,2017,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "internal/types.hxx"

JSONInt::JSONInt(int64_t intValue) : JSONAtom(JSON_TYPE_INT), value(intValue)
{
}

JSONInt::~JSONInt()
{
}

JSONInt::operator int64_t() const
{
	return value;
}

void JSONInt::set(int64_t intValue)
{
	value = intValue;
}
