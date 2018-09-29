/*
 * This file is part of rSON
 * Copyright © 2012-2013 Rachel Mant (dx-mon@users.sourceforge.net)
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
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

std::unique_ptr<const char []> vaFormatString(const char *const format, va_list args) noexcept
{
	va_list lenArgs;
	va_copy(lenArgs, args);
	const size_t len = vsnprintf(NULL, 0, format, lenArgs) + 1;
	va_end(lenArgs);
	auto ret = makeUnique<char []>(len);
	if (!ret)
		return nullptr;
	vsprintf(ret.get(), format, args);
	return std::unique_ptr<const char []>(ret.release());
}

std::unique_ptr<const char []> formatString(const char *const format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	auto ret = vaFormatString(format, args);
	va_end(args);
	return ret;
}

size_t formatLen(const char *const format, ...)
{
	size_t len;
	va_list args;
	va_start(args, format);
	len = vsnprintf(NULL, 0, format, args);
	va_end(args);
	return len;
}

std::unique_ptr<char []> stringDup(const char *const str)
{
	// TODO: Change this to use makeUniqueT<>, a throwing version of this
	auto ret = makeUnique<char []>(strlen(str) + 1);
	if (!ret)
		throw std::bad_alloc();
	strcpy(ret.get(), str);
	return ret;
}

char *strNewDup(const char *str)
{
	char *ret = new char[strlen(str) + 1];
	strcpy(ret, str);
	return ret;
}
