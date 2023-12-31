/*
 * This file is part of rSON
 * Copyright © 2019 Rachel Mant (dx-mon@users.sourceforge.net)
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
