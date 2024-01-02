// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2017,2019,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "test.h"
#include "testHeader.h"

void testNotImplemented()
{
	try
	{
		throwNotImplemented();
		fail("notImplemented_t exception not thrown");
	}
	catch (notImplemented_t &) { }
}

void tryStreamFail(streamTest_t &stream, void operation(streamTest_t &))
{
	try
	{
		operation(stream);
		fail("Should have thrown notImplemented_t exception");
	}
	catch (notImplemented_t &) { }
}

void testStreamType()
{
	streamTest_t stream{};
	tryStreamFail(stream, [](streamTest_t &stream) { stream.read(); });
	tryStreamFail(stream, [](streamTest_t &stream) { stream.write(); });
	tryStreamFail(stream, [](streamTest_t &stream) { stream.atEOF(); });
}

void testReadStream()
{
	readStream();
	badReadStream();
}

void testWriteArray() { writeArray(); }

extern "C"
{
BEGIN_REGISTER_TESTS()
	TEST(testNotImplemented)
	TEST(testStreamType)
	TEST(testReadStream)
	TEST(testWriteArray)
END_REGISTER_TESTS()
}
