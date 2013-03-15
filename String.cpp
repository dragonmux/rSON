/*
 * This file is part of rSON
 * Copyright © 2012 Richard/Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * rSON is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rSON is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "String.h"
#include "Memory.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

char *formatString(const char *format, ...)
{
	size_t len;
	char *ret;
	va_list args;
	va_start(args, format);
	len = vsnprintf(NULL, 0, format, args);
	va_end(args);
	ret = (char *)memMalloc(len + 1);
	va_start(args, format);
	vsprintf(ret, format, args);
	va_end(args);
	return ret;
}

size_t formatLen(const char *format, ...)
{
	size_t len;
	va_list args;
	va_start(args, format);
	len = vsnprintf(NULL, 0, format, args);
	va_end(args);
	return len;
}
