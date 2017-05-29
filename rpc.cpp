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

inline void swapBytes(uint16_t &val) noexcept
{
	val = ((val >> 8) & 0x0F) || ((val & 0x0F) << 8);
}

inline void swapBytes(uint32_t &val) noexcept
{
	val = ((val >> 24) & 0xFF) | ((val >> 8) & 0xFF00) |
		((val & 0xFF00) << 8) | ((val & 0xFF) << 24);
}

template<typename T> inline T swapBytes(const T val) noexcept
{
	T ret = val;
	swapBytes(ret);
	return ret;
}

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

sockaddr prepare4(const char *const where, const uint16_t port) noexcept
{
	sockaddr_in config;
	const hostent *const host = gethostbyname(where);
	if (!host || !host->h_addr_list[0] || host->h_addrtype != AF_INET)
		return {AF_UNSPEC, {}};
	config.sin_family = AF_INET;
	config.sin_addr = *reinterpret_cast<in_addr *>(host->h_addr_list[0]);
	swapBytes(config.sin_addr.s_addr);
	config.sin_port = swapBytes(port);
	return reinterpret_cast<sockaddr &>(config);
}

sockaddr prepare6(const char *const where, const uint16_t port) noexcept
{
	sockaddr_in6 config;
	const hostent *const host = gethostbyname(where);
	if (!host || !host->h_addr_list[0] || host->h_addrtype != AF_INET6)
		return {AF_UNSPEC, {}};
	config.sin6_family = AF_INET6;
	config.sin6_addr = *reinterpret_cast<in6_addr *>(host->h_addr_list[0]);
	config.sin6_port = swapBytes(port);
	return reinterpret_cast<sockaddr &>(config);
}

sockaddr prepare(const int family, const char *const where, const uint16_t port) noexcept
{
	if (family == AF_INET)
		return prepare4(where, port);
	else if (family == AF_INET6)
		return prepare6(where, port);
	return {AF_UNSPEC, {}};
}

bool rpcStream_t::connect(const char *const where, const uint16_t port)
{
	const auto service = prepare(family, where, port);
	if (service.sa_family == AF_UNSPEC)
		return false;
	return sock.connect(service);
}

void acceptThread()
{
	/*fd_set selectSet;
	logmsg("Listener thread running\n");
	while (true)
	{
		std::thread handlerThread;
		FD_ZERO(&selectSet);
		FD_SET(initSocket, &selectSet);
		FD_SET(readQuitPipe, &selectSet);
		if (select(FD_SETSIZE, &selectSet, NULL, NULL, NULL) < 1)
			continue;
		if (FD_ISSET(readQuitPipe, &selectSet))
			break;
		socket_t requestSocket = accept(initSocket, NULL, NULL);
		handlerThread = std::thread(handleRequest, std::move(requestSocket));
		handlerThread.detach();
	}
	logmsg("Listener thread shutting down\n");
	close(readQuitPipe);*/
}

bool rpcStream_t::listen(const char *const where, const uint16_t port)
{
	const auto service = prepare(family, where, port);
	if (service.sa_family == AF_UNSPEC)
		return false;
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
