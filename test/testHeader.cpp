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
	streamTest_t stream;
	tryStreamFail(stream, [](streamTest_t &stream) { stream.read(); });
	tryStreamFail(stream, [](streamTest_t &stream) { stream.write(); });
	tryStreamFail(stream, [](streamTest_t &stream) { stream.atEOF(); });
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testNotImplemented)
	TEST(testStreamType)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
