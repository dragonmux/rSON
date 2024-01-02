// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2014,2017-2018,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include <crunch.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "rSON.hxx"
using namespace rSON;

#define WANTED_TYPE(whatToDo) \
try \
{ \
	whatToDo; \
} \
catch (const JSONTypeError &e) \
{ \
	fail(e.error()); \
}

#define UNWANTED_TYPE(onWhat, type) \
try \
{ \
	onWhat->as ## type (); \
	fail("Type " #type " converted even though wrong"); \
} \
catch (const JSONTypeError &e) \
{ \
}
