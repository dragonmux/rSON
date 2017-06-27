#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
//#include <atomic>

#include "../rSON.h"
#include "../rpc.h"

extern sockaddr prepare(const int family, const char *const where, const uint16_t port) noexcept;

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
	}
	bool atEOF() const noexcept final override { return false; }
};

bool rxThread()
{
	rpcStream_t listener(false);
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
		printf("Failure: %s\n", e.error());
		readyCond.notify_all();
		return false;
	}
	readyCond.notify_all();
	return true;
}

bool txThread()
{
	rpcStream_t stream(false);
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
		printf("Failure: %s\n", e.error());
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
	const auto service = prepare(AF_INET, "localhost", 2010);
	if (service.sa_family == AF_UNSPEC)
		return 1;
	else if (service.sa_family == AF_INET)
	{
		const auto &addr = reinterpret_cast<const sockaddr_in &>(service).sin_addr.s_addr;
		printf("localhost address: %u.%u.%u.%u\n", (addr >> 0) & 0xFF, (addr >> 8) & 0xFF,
			(addr >> 16) & 0xFF, (addr >> 24) & 0xFF);
	}
	else if (service.sa_family == AF_INET6)
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

