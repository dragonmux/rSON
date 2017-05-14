/*
 * This file is part of rSON
 * Copyright © 2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "internal.h"
#include "rpc.h"

void acceptThread();

socket_t::socket_t(const int family, const int type, const int protocol) noexcept :
	socket(::socket(family, type, protocol)) { }
bool socket_t::bind(const void *const addr, const size_t len) const noexcept
	{ return ::bind(socket, reinterpret_cast<const sockaddr *>(addr), len) == 0; }
bool socket_t::connect(const void *const addr, const size_t len) const noexcept
	{ return ::connect(socket, reinterpret_cast<const sockaddr *>(addr), len) == 0; }
bool socket_t::listen(const int32_t queueLength) const noexcept
	{ return ::listen(socket, queueLength) == 0; }
socket_t socket_t::accept(sockaddr *peerAddr, socklen_t *peerAddrLen) const noexcept
	{ return ::accept(socket, peerAddr, peerAddrLen); }
ssize_t socket_t::write(const void *const bufferPtr, const size_t len) const noexcept
	{ return ::write(socket, bufferPtr, len); }
ssize_t socket_t::read(void *const bufferPtr, const size_t len) const noexcept
{
	size_t num = 0;
	char *const buffer = static_cast<char *const>(bufferPtr);
	do
	{
		ssize_t res = ::read(socket, buffer + num, len - num);
		if (res <= 0)
			return -1;
		num += res;
	}
	while (num < len);
	return num;
}

rpcStream_t::rpcStream_t(const bool ipv6) : family(ipv6 ? AF_INET6 : AF_INET),
	sock(family, SOCK_STREAM, IPPROTO_TCP), threadAccept() { }

sockaddr_in prepare(const int family, const char *const where, const uint16_t port) noexcept
{
	sockaddr_in config;
	config.sin_family = family;
	//config.sin_addr.s_addr =
	config.sin_port = htons(port);
	return config;
}

bool rpcStream_t::connect(const char *const where, const uint16_t port)
{
	const auto service = prepare(family, where, port);
	return sock.connect(service);
}

bool rpcStream_t::listen(const char *const where, const uint16_t port)
{
	const auto service = prepare(family, where, port);
	const bool ok = sock.bind(service) && sock.listen(1);
	threadAccept = std::thread(acceptThread);
	return ok;
}

bool rpcStream_t::read(void *const value, const size_t valueLen, size_t &actualLen)
{
	actualLen = 0;
	const ssize_t result = sock.read(value, valueLen);
	if (result > 0)
		actualLen = size_t(result);
	return actualLen == valueLen;
}

bool rpcStream_t::write(const void *const value, const size_t valueLen)
{
	const ssize_t result = sock.write(value, valueLen);
	if (result < 0)
		return false;
	return size_t(result) == valueLen;
}

void acceptThread()
{
}
