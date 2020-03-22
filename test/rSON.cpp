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

// This file's mission in life is to provide code coverage
// for the rSON header file. This implements a series of
// wrappers for testing and must be compiled the same
// as the library's own sources so it gets included in gcov.

#include "test.h"
#include "testHeader.h"

void throwNotImplemented() { throw notImplemented_t{}; }

bool streamTest_t::read()
{
	size_t actualLen = 0;
	return stream.read(nullptr, 0, actualLen);
}

bool streamTest_t::write() { return stream.write(nullptr, 0); }
bool streamTest_t::atEOF() const { return stream.atEOF(); }

void readStream()
{
	std::array<char, 1> dest;
	const char *const testData = "a";
	memoryStream_t srcStream(const_cast<char *>(testData), strlen(testData));
	stream_t &stream = srcStream;

	assertFalse(stream.atEOF());
	assertTrue(stream.read(dest));
	assertTrue(stream.atEOF());
	assertIntEqual(dest[0], testData[0]);
	assertFalse(stream.read(dest));
}

void badReadStream()
{
	std::array<char, 2> dest;
	const char *const testData = "abc";
	memoryStream_t srcStream(const_cast<char *>(testData), strlen(testData));
	stream_t &stream = srcStream;

	assertFalse(stream.atEOF());
	assertTrue(stream.read(dest));
	assertFalse(stream.atEOF());
	assertFalse(stream.read(nullptr, -1));
}

void writeArray()
{
	std::array<char, 3> dest;
	std::array<char, 2> testData{'a', 'b'};
	memoryStream_t srcStream(dest.data(), dest.size());
	stream_t &stream = srcStream;

	assertFalse(stream.atEOF());
	assertTrue(stream.write(testData));
	assertFalse(stream.atEOF());
	assertMemEqual(dest.data(), testData.data(), testData.size());
	assertFalse(stream.write(nullptr, -1));
	assertFalse(stream.atEOF());
	assertFalse(stream.write(testData));
	assertTrue(stream.atEOF());
	assertFalse(stream.write(testData));
}
