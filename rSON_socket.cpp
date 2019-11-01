/*
 * This file is part of rSON
 * Copyright © 2017-2018 Rachel Mant (dx-mon@users.sourceforge.net)
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

#ifndef _MSC_VER
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include "internal.h"
#include "rSON_socket.h"
#include "String.hxx"

#ifdef __GNUC__
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define SWAP 1
#else
#define SWAP 0
#endif
#elif defined(_MSC_VER)
// This is wrong.. but it's better than nothing for MSVC.
#define SWAP 1
#endif

#ifndef _MSC_VER
#include <unistd.h>
inline int closesocket(const int s) { return close(s); }
#else
#include <Winsock2.h>
#endif

inline uint16_t swapBytes(const uint16_t val) noexcept
{
#if SWAP
	return ((val >> 8) & 0xFF) | ((val & 0xFF) << 8);
#endif
}

inline uint32_t swapBytes(const uint32_t val) noexcept
{
#if SWAP
	return ((val >> 24) & 0xFF) | ((val >> 8) & 0xFF00) |
		((val & 0xFF00) << 8) | ((val & 0xFF) << 24);
#endif
}

template<typename T> inline void swapBytes(T &val) noexcept
	{ val = swapBytes(val); }

size_t sockaddrLen(const sockaddr_storage &addr) noexcept
{
	switch (addr.ss_family)
	{
		case AF_INET:
			return sizeof(sockaddr_in);
		case AF_INET6:
			return sizeof(sockaddr_in6);
	}
	return sizeof(sockaddr);
}

socket_t::socket_t(const int family, const int type, const int protocol) noexcept :
	socket(::socket(family, type, protocol)) { }
socket_t::~socket_t() noexcept
	{ if (socket != -1) closesocket(socket); }
bool socket_t::bind(const void *const addr, const size_t len) const noexcept
	{ return ::bind(socket, static_cast<const sockaddr *>(addr), len) == 0; }
bool socket_t::bind(const sockaddr_storage &addr) const noexcept
	{ return bind(static_cast<const void *>(&addr), sockaddrLen(addr)); }
bool socket_t::connect(const void *const addr, const size_t len) const noexcept
	{ return ::connect(socket, static_cast<const sockaddr *>(addr), len) == 0; }
bool socket_t::connect(const sockaddr_storage &addr) const noexcept
	{ return connect(static_cast<const void *>(&addr), sockaddrLen(addr)); }
bool socket_t::listen(const int32_t queueLength) const noexcept
	{ return ::listen(socket, queueLength) == 0; }
socket_t socket_t::accept(sockaddr *peerAddr, socklen_t *peerAddrLen) const noexcept
	{ return ::accept(socket, peerAddr, peerAddrLen); }
#ifndef _MSC_VER
ssize_t socket_t::write(const void *const bufferPtr, const size_t len) const noexcept
	{ return ::write(socket, bufferPtr, len); }
ssize_t socket_t::read(void *const bufferPtr, const size_t len) const noexcept
	{ return ::read(socket, bufferPtr, len); }
#else
ssize_t socket_t::write(const void *const bufferPtr, const size_t len) const noexcept
	{ return ::send(socket, static_cast<const char *const>(bufferPtr), int32_t(len), 0); }
ssize_t socket_t::read(void *const bufferPtr, const size_t len) const noexcept
	{ return ::recv(socket, static_cast<char *const>(bufferPtr), int32_t(len), 0); }
#endif

char socket_t::peek() const noexcept
{
	char buffer;
	if (::recv(socket, &buffer, 1, MSG_PEEK) != 1)
		return '\n';
	return buffer;
}

int typeToFamily(const socketType_t type) noexcept
{
	if (type == socketType_t::ipv4)
		return AF_INET;
	else if (type == socketType_t::ipv6)
		return AF_INET6;
	return AF_UNSPEC;
}

rpcStream_t::rpcStream_t() noexcept : family(socketType_t::unknown), sock(), buffer(), pos(0), lastRead(0) { }
bool rpcStream_t::valid() const noexcept { return family != socketType_t::unknown && sock != -1; }
void rpcStream_t::makeBuffer() noexcept { buffer = makeUnique<char []>(bufferLen); }

rpcStream_t::rpcStream_t(const socketType_t type) : family(type), sock(), buffer(),
	pos(0), lastRead(0)
{
	if (type != socketType_t::unknown && type != socketType_t::dontCare)
		sock = socket_t(typeToFamily(family), SOCK_STREAM, IPPROTO_TCP);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
sockaddr_storage prepare(const socketType_t family, const char *const where, const uint16_t port) noexcept
{
	addrinfo *results = nullptr, hints = {};
	hints.ai_family = typeToFamily(family);
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; // This may not be right/complete..

	if (getaddrinfo(where, nullptr, &hints, &results) || !results)
		return {AF_UNSPEC};

	sockaddr_storage service{};
	memcpy(&service, &results->ai_addr, sizeof(sockaddr_storage));
	freeaddrinfo(results);

	if (service.ss_family == AF_INET)
	{
		auto &addr = reinterpret_cast<sockaddr_in &>(service); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) lgtm[cpp/reinterpret-cast]
		addr.sin_port = swapBytes(port);
	}
	else if (service.ss_family == AF_INET6)
	{
		auto &addr = reinterpret_cast<sockaddr_in6 &>(service); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) lgtm[cpp/reinterpret-cast]
		addr.sin6_port = swapBytes(port);
	}
	else
		return {AF_UNSPEC};
	return service;
}
#pragma GCC diagnostic pop

bool rpcStream_t::connect(const char *const where, const uint16_t port) noexcept
{
	const auto service = prepare(family, where, port);
	if (service.ss_family == AF_UNSPEC)
		return false;
	else if (family == socketType_t::dontCare)
		sock = socket_t(service.ss_family, SOCK_STREAM, IPPROTO_TCP);
	const_cast<rpcStream_t &>(*this).makeBuffer();
	return sock.connect(service);
}

bool rpcStream_t::listen(const char *const where, const uint16_t port) noexcept
{
	const auto service = prepare(family, where, port);
	if (service.ss_family == AF_UNSPEC)
		return false;
	else if (family == socketType_t::dontCare)
		sock = socket_t(service.ss_family, SOCK_STREAM, IPPROTO_TCP);
	return sock.bind(service) && sock.listen(1);
}

rpcStream_t rpcStream_t::accept() const noexcept
{
	fd_set selectSet;
	FD_ZERO(&selectSet);
	FD_SET(sock, &selectSet);
	if (select(FD_SETSIZE, &selectSet, nullptr, nullptr, nullptr) < 1)
		return {};
	return {family, sock.accept(nullptr, nullptr), makeUnique<char []>(bufferLen)};
}

bool rpcStream_t::read(void *const value, const size_t valueLen, size_t &actualLen)
{
	actualLen = 0;
	const ssize_t result = sock.read(value, valueLen);
	if (result > 0)
	{
		actualLen = size_t(result);
		lastRead = static_cast<char *const>(value)[actualLen - 1];
	}
	return actualLen == valueLen;
}

bool rpcStream_t::write(const void *const valuePtr, const size_t valueLen)
{
	if (!buffer)
		return false;
	auto value = static_cast<const char *const>(valuePtr);
	size_t offs = 0, toWrite = valueLen, written = 0;
	while (toWrite > 0)
	{
		const uint32_t length = (bufferLen - pos) < toWrite ? bufferLen - pos : toWrite;
		memcpy(buffer.get() + pos, value + offs, length);
		offs += length;
		pos += length;
		toWrite -= length;
		if (pos == bufferLen)
		{
			const ssize_t result = sock.write(buffer.get(), bufferLen);
			if (result < 0)
				return false;
			pos = 0;
			written += size_t(result);
		}
		else
			written += length;
	}
	return written == valueLen;
}

void rpcStream_t::readSync() noexcept
{
	char discard;
	while (lastRead != '\n' && sock.peek() != '\n')
		sock.read(&discard, 1);
	if (lastRead != '\n')
		sock.read(&discard, 1);
	lastRead = 0;
}

void rpcStream_t::writeSync() noexcept
{
	write("\n", 1);
	if (pos > 0)
		sock.write(buffer.get(), pos);
	pos = 0;
}

bool rpcStream_t::atEOF() const noexcept
	{ return lastRead == '\n'; }
