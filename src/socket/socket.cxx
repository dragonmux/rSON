// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2017-2020,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileCopyrightText: 2021 Amyspark <amy@amyspark.me>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Modified by Amyspark <amy@amyspark.me>

#ifndef _WIN32
#include <sys/select.h>
#endif
#include <substrate/utility>
#include "rSONSocket.hxx"

#ifndef _WIN32
using substrate::INVALID_SOCKET;
#endif
using substrate::ssize_t;
using namespace rSON;

rpcStream_t::rpcStream_t() noexcept : family(socketType_t::unknown), sock(), buffer(), pos(0), lastRead(0) { }
bool rpcStream_t::valid() const noexcept { return family != socketType_t::unknown && sock != INVALID_SOCKET; }
void rpcStream_t::makeBuffer() noexcept { buffer = substrate::make_unique<char []>(bufferLen); }

rpcStream_t::rpcStream_t(const socketType_t type) : family(type), sock(), buffer(),
	pos(0), lastRead(0)
{
	if (type != socketType_t::unknown && type != socketType_t::dontCare)
		sock = {
			substrate::socket::typeToFamily(family),
			substrate::socket::protocolToType(substrate::socketProtocol_t::tcp),
			substrate::socket::protocolToHints(substrate::socketProtocol_t::tcp)
		};
}

bool rpcStream_t::connect(const char *const where, const uint16_t port) noexcept
{
	const auto service = substrate::socket::prepare(family, where, port);
	if (service.ss_family == AF_UNSPEC)
		return false;
	else if (family == socketType_t::dontCare)
		sock = {
			service.ss_family,
			substrate::socket::protocolToType(substrate::socketProtocol_t::tcp),
			substrate::socket::protocolToHints(substrate::socketProtocol_t::tcp)
		};
	const_cast<rpcStream_t &>(*this).makeBuffer();
	return sock.connect(service);
}

bool rpcStream_t::listen(const char *const where, const uint16_t port) noexcept
{
	const auto service = substrate::socket::prepare(family, where, port);
	if (service.ss_family == AF_UNSPEC)
		return false;
	else if (family == socketType_t::dontCare)
		sock = {
			service.ss_family,
			substrate::socket::protocolToType(substrate::socketProtocol_t::tcp),
			substrate::socket::protocolToHints(substrate::socketProtocol_t::tcp)
		};
	return sock.bind(service) && sock.listen(1);
}

rpcStream_t rpcStream_t::accept() const noexcept
{
	fd_set selectSet;
	FD_ZERO(&selectSet);
	FD_SET(sock, &selectSet);
	if (select(FD_SETSIZE, &selectSet, nullptr, nullptr, nullptr) < 1)
		return {};
	return {family, sock.accept(nullptr, nullptr), substrate::make_unique<char []>(bufferLen)};
}

bool rpcStream_t::read(void *const value, const size_t valueLen, size_t &actualLen)
{
	actualLen = 0;
	const ssize_t result = sock.read(value, valueLen);
	if (result > 0)
	{
		actualLen = size_t(result);
		lastRead = static_cast<char *>(value)[actualLen - 1];
	}
	return actualLen == valueLen;
}

bool rpcStream_t::write(const void *const valuePtr, const size_t valueLen)
{
	if (!buffer)
		return false;
	const auto value = static_cast<const char *>(valuePtr);
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
