// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2014,2017-2018,2020,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#ifndef INTERNAL_STRING_HXX
#define INTERNAL_STRING_HXX

#include <stdlib.h>
#include <memory>

std::unique_ptr<const char []> formatString(const char *const format, ...) noexcept;
extern size_t formatLen(const char *const format, ...);
extern char *strNewDup(const char *const str);
extern std::unique_ptr<char []> stringDup(const char *const str);

#endif /*INTERNAL_STRING_HXX*/
