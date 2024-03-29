// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2016-2018,2020,2023-2024 Rachel Mant <git@dragonmux.network>
// SPDX-FileCopyrightText: 2021 Amyspark <amy@amyspark.me>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Modified by Amyspark <amy@amyspark.me>

#include <sys/stat.h>
#include <stddef.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#include <type_traits>
using ssize_t = typename std::make_signed<size_t>::type;
#endif
#include <errno.h>
#include <system_error>

#include "internal/types.hxx"

fileStream_t::fileStream_t(const char *const fileName, const int32_t _mode, const int32_t perms) : mode{_mode}
{
	struct stat fileStat;
	fd = open(fileName, mode, perms);
	if (fd == -1 || fstat(fd, &fileStat) != 0)
		throw std::system_error{errno, std::system_category()};
	length = fileStat.st_size;
}

#ifdef _WIN32
fileStream_t::fileStream_t(const wchar_t *const fileName, const int32_t _mode, const int32_t perms) : mode{_mode}
{
	struct stat fileStat;
	fd = _wopen(fileName, mode, perms);
	if (fd == -1 || fstat(fd, &fileStat) != 0)
		throw std::system_error{errno, std::system_category()};
	length = fileStat.st_size;
}
#endif

fileStream_t::~fileStream_t() noexcept { close(fd); }

bool fileStream_t::read(void *const value, const size_t valueLen, size_t &actualLen)
{
	// If write-only and not read-write mode, or we got to eof.. return false.
	if (eof || ((mode & O_WRONLY) && !(mode & O_RDWR)))
		return false;
#ifndef _MSC_VER
	ssize_t ret = ::read(fd, value, valueLen);
#else
	ssize_t ret = ::read(fd, value, uint32_t(valueLen));
#endif
	if (ret < 0)
		throw std::system_error(errno, std::system_category());
	eof = ret == 0;
	actualLen = size_t(ret);
	return true;
}

bool fileStream_t::write(const void *const value, const size_t valueLen)
{
	// If read-only and not read-write mode, or we ran out of space.. return false.
	if (eof || !(mode & (O_WRONLY | O_RDWR)))
		return false;
#ifndef _MSC_VER
	ssize_t ret = ::write(fd, value, valueLen);
#else
	ssize_t ret = ::write(fd, value, uint32_t(valueLen));
#endif
	if (ret < 0)
		throw std::system_error(errno, std::system_category());
	eof = ret == 0;
	length = lseek(fd, 0, SEEK_CUR);
	return !eof;
}

off_t fileStream_t::seek(const off_t offset, const int whence) noexcept
{
	off_t result = lseek(fd, offset, whence);
	eof = length == size_t(result);
	return result;
}

memoryStream_t::memoryStream_t(void *const stream, const size_t streamLength) noexcept :
	memory(static_cast<char *>(stream)), length(streamLength), pos(0) { }

bool memoryStream_t::read(void *const value, const size_t valueLen, size_t &actualLen) noexcept
{
	// If at "end of file", or the requested read would cause position wrap around, return false.
	if (atEOF() || (pos + valueLen) < pos)
		return false;
	// Calculate how much we can really read vs requested.
	actualLen = (pos + valueLen) > length ? length - pos : valueLen;
	memcpy(value, memory + pos, actualLen);
	pos += actualLen;
	return true;
}

bool memoryStream_t::write(const void *const value, const size_t valueLen) noexcept
{
	// If at "end of file", or the requested write would cause position wrap-around, return false.
	if (atEOF() || (pos + valueLen) < pos)
		return false;
	// Calculate how much we can really write vs requested.
	const size_t actualLen = (pos + valueLen) > length ? length - pos : valueLen;
	memcpy(memory + pos, value, actualLen);
	pos += actualLen;
	// If we did not acomplish a complete write, we consider that a failure.
	return valueLen == actualLen;
}
