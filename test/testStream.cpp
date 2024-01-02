// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2019,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "test.h"
#include "testStream.h"
#include <memory>

void tryStreamFail(stream_t &stream, bool operation(stream_t &))
{
	try
	{
		operation(stream);
		fail("Should have thrown notImplemented_t exception");
	}
	catch (notImplemented_t &) { }
}

void testStreamThrows()
{
	stream_t stream{};
	tryStreamFail(stream, writeStream);
	tryStreamFail(stream, readStream);
	tryStreamFail(stream, streamAtEOF);
}

void testStreamNOPs()
{
	auto stream = std::make_unique<stream_t>();
	assertNotNull(stream.get());
	streamReadSync(*stream);
	streamWriteSync(*stream);
	streamDelete(stream.release());
}

extern "C"
{
BEGIN_REGISTER_TESTS()
	TEST(testStreamThrows)
	TEST(testStreamNOPs)
END_REGISTER_TESTS()
}
