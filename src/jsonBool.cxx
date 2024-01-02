// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2014,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "internal/types.hxx"

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

void JSONBool::set(bool boolValue)
{
	value = boolValue;
}
