// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2018,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>

#include "test.h"
#include "socket/rSON_socket.hxx"

extern size_t sockaddrLen(const sockaddr_storage &addr) noexcept;
extern sockaddr_storage prepare(const socketType_t family, const char *const where, const uint16_t port) noexcept;

// This is a schema taken from http://json-schema.org/example1.html
const char *const testJSON = "{\"$schema\":\"http://json-schema.org/draft-04/schema#\",\"title\":\"Product\",\"description\":\"A product from Acme's catalog\","
	"\"type\":\"object\",\"properties\":{\"id\":{\"description\":\"The unique identifier for a product\",\"type\":\"integer\"},\"name\":{\"description\":"
	"\"Name of the product\",\"type\":\"string\"},\"price\":{\"type\":\"number\",\"minimum\":0,\"exclusiveMinimum\":true},\"tags\":{\"type\":\"array\","
	"\"items\":{\"type\":\"string\"},\"minItems\":1,\"uniqueItems\":true}},\"required\":[\"id\",\"name\",\"price\"]}";
std::mutex readyMutex, exitMutex;
std::condition_variable readyCond;
bool ready = false;
using namespace rSON;

struct printStream_t final : public stream_t
{
	printStream_t() = default;
	~printStream_t() { fputc('\n', stdout); }

	bool write(const void *const buffer, const size_t stringLen) noexcept final override
	{
		const char *const string = static_cast<const char *const>(buffer);
		for (size_t i = 0; i < stringLen; ++i)
			fputc(string[i], stdout);
		return true;
	}
	bool atEOF() const noexcept final override { return false; }
};

struct unspecStream_t final : public rpcStream_t
{
public:
	unspecStream_t() : rpcStream_t() { }
};

bool rxThread()
{
	rpcStream_t listener(socketType_t::dontCare);
	if (listener.listen("localhost", 2010))
		ready = true;
	readyCond.notify_all();
	if (!ready)
	{
		printf("Failed to listen to localhost:2010 - %d, %s\n", errno, strerror(errno));
		return false;
	}
	rpcStream_t stream = listener.accept();

	{
		std::unique_lock<std::mutex> lock(readyMutex);
		readyCond.wait(lock);
		if (!ready)
			return false;
	}
	printf("Trying to recieve\n");
	try
	{
		JSONAtom *tree = parseJSON(stream);
		printStream_t outStream;
		writeJSON(tree, outStream);
	}
	catch (JSONParserError &e)
	{
		printf("Read failure: %s\n", e.error());
		readyCond.notify_all();
		return false;
	}
	readyCond.notify_all();
	return true;
}

bool txThread()
{
	rpcStream_t stream(socketType_t::dontCare);
	{
		std::unique_lock<std::mutex> lock(readyMutex);
		readyCond.wait(lock);
		if (!ready)
			return false;
	}
	if (!stream.connect("localhost", 2010))
	{
		ready = false;
		printf("Failed to connect to localhost:2010 - %d, %s\n", errno, strerror(errno));
		readyCond.notify_all();
		return false;
	}

	JSONAtom *tree = nullptr;
	try
	{
		memoryStream_t data((void *)testJSON, strlen(testJSON) + 1);
		tree = parseJSON(data);
		if (!tree)
			ready = false;
	}
	catch (JSONParserError &e)
	{
		printf("Write failure: %s\n", e.error());
		ready = false;
	}
	readyCond.notify_all();
	if (!ready)
		return false;
	printf("Sending\n");
	writeJSON(tree, stream);
	delete tree;
	return true;
}

int main(int, char **) noexcept
{
	std::unique_lock<std::mutex> lock(exitMutex);
	const auto service = prepare(socketType_t::ipv4, "localhost", 2010);
	if (service.ss_family == AF_UNSPEC)
		return 1;
	else if (service.ss_family == AF_INET)
	{
		const auto &addr = reinterpret_cast<const sockaddr_in &>(service).sin_addr.s_addr;
		printf("localhost address: %u.%u.%u.%u\n", (addr >> 0) & 0xFF, (addr >> 8) & 0xFF,
			(addr >> 16) & 0xFF, (addr >> 24) & 0xFF);
	}
	else if (service.ss_family == AF_INET6)
	{
		const auto &addr = reinterpret_cast<const sockaddr_in6 &>(service).sin6_addr.s6_addr;
		printf("localhost address: ");
		printf("%x:%x:%x:%x:%x:%x:%x:%x:", addr[0], addr[1], addr[2], addr[3],
			addr[4], addr[5], addr[6], addr[7]);
		printf("%x:%x:%x:%x:%x:%x:%x:%x\n", addr[8], addr[9], addr[10], addr[11],
			addr[12], addr[13], addr[14], addr[15]);
	}

	auto txResult = std::async(std::launch::async, txThread);
	auto rxResult = std::async(std::launch::async, rxThread);
	txResult.wait();
	rxResult.wait();

	return txResult.get() && rxResult.get() ? 0 : 1;
}

void testPrepare()
{
	auto service4 = prepare(socketType_t::ipv4, "127.0.0.1", 2010);
	assertIntEqual(service4.ss_family, AF_INET);
	assertIntEqual(sockaddrLen(service4), sizeof(sockaddr_in));
	service4 = prepare(socketType_t::ipv4, "", 2010);
	assertIntEqual(service4.ss_family, AF_UNSPEC);
	service4 = prepare(socketType_t::ipv4, nullptr, 2010);
	assertIntEqual(service4.ss_family, AF_UNSPEC);
	assertIntEqual(sockaddrLen(service4), sizeof(sockaddr));

	auto service6 = prepare(socketType_t::ipv6, "::1", 2010);
	assertIntEqual(service6.ss_family, AF_INET6);
	assertIntEqual(sockaddrLen(service6), sizeof(sockaddr_in6));
	service6 = prepare(socketType_t::ipv6, "", 2010);
	assertIntEqual(service6.ss_family, AF_UNSPEC);
	service6 = prepare(socketType_t::ipv6, nullptr, 2010);
	assertIntEqual(service6.ss_family, AF_UNSPEC);
	assertIntEqual(sockaddrLen(service4), sizeof(sockaddr));

//	assertIntEqual(unspecStream_t().prepare("", 0).ss_family, AF_UNSPEC);
}

extern "C"
{
BEGIN_REGISTER_TESTS()
	TEST(testPrepare)
END_REGISTER_TESTS()
}
