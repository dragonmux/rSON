/*
 * This file is part of rSON
 * Copyright © 2016-2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

fileStream_t::fileStream_t(const char *const fileName, const int32_t _mode, const int32_t perms) : fd(-1), eof(false), mode(_mode)
{
	struct stat fileStat;
	fd = open(fileName, mode, perms);
	if (fd == -1 || fstat(fd, &fileStat) != 0)
		throw std::system_error(errno, std::system_category());
	length = fileStat.st_size;
}

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

off_t fileStream_t::seek(const off_t offset, const int whence) rSON_NOEXCEPT
{
	off_t result = lseek(fd, offset, whence);
	eof = length == size_t(result);
	return result;
}

memoryStream_t::memoryStream_t(void *const stream, const size_t streamLength) rSON_NOEXCEPT :
	memory(static_cast<char *>(stream)), length(streamLength), pos(0) { }

bool memoryStream_t::read(void *const value, const size_t valueLen, size_t &actualLen) rSON_NOEXCEPT
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

bool memoryStream_t::write(const void *const value, const size_t valueLen) rSON_NOEXCEPT
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
