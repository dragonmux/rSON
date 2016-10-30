#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <system_error>

#include "internal.h"

fileStream_t::fileStream_t(const char *const fileName, const int32_t mode) : fd(-1), eof(false)
{
	struct stat fileStat;
	fd = open(fileName, mode);
	if (fd == -1 || fstat(fd, &fileStat) != 0)
		throw std::system_error(errno, std::system_category());
	length = fileStat.st_size;
}

fileStream_t::~fileStream_t() noexcept { close(fd); }

bool fileStream_t::read(void *const value, const size_t valueLen, size_t &actualLen)
{
	// If write-only and not read-write mode, or we got to eof.. return false.
	if (eof || (mode & O_WRONLY) || !(mode & O_RDWR))
		return false;
	ssize_t ret = ::read(fd, value, valueLen);
	if (ret < 0)
		throw std::system_error(errno, std::system_category());
	eof = length == size_t(lseek(fd, 0, SEEK_CUR));
	actualLen = size_t(ret);
	return true;
}

bool fileStream_t::write(const void *const value, const size_t valueLen)
{
	// If read-only and not read-write mode, or we ran out of space.. return false.
	if (eof || !(mode & (O_WRONLY | O_RDWR)))
		return false;
	ssize_t ret = ::write(fd, value, valueLen);
	if (ret < 0)
		throw std::system_error(errno, std::system_category());
	eof = ret == 0;
	return true;
}

memoryStream_t::memoryStream_t(void *const stream, const size_t streamLength) noexcept :
	memory(static_cast<char *const>(stream)), length(streamLength), pos(0) { }

bool memoryStream_t::read(void *const value, const size_t valueLen, size_t &actualLen) noexcept
{
	if (atEOF() || (pos + valueLen) < pos)
		return false;
	actualLen = (pos + valueLen) > length ? length - pos : valueLen;
	memcpy(value, memory + pos, actualLen);
	pos += actualLen;
	return true;
}
