// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2017-2020 Rachel Mant <git@dragonmux.network>
// SPDX-FileCopyrightText: 2023 Aki Van Ness <aki@lethalbit.net>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Modified by Aki Van Ness <aki@lethalbit.net>

#include <array>

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
