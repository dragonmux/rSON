// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2014,2017-2018,2020,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <substrate/utility>
#include "internal/string.hxx"

std::unique_ptr<const char []> vaFormatString(const char *const format, va_list args) noexcept
{
	va_list lenArgs;
	va_copy(lenArgs, args);
	const size_t len = vsnprintf(NULL, 0, format, lenArgs) + 1;
	va_end(lenArgs);
	auto ret = substrate::make_unique_nothrow<char []>(len);
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
	auto ret = substrate::make_unique<char []>(strlen(str) + 1);
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
