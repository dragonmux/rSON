// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2019 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "test.h"
#include "testStream.h"

bool writeStream(stream_t &stream)
	{ return stream.write(nullptr, 0); }

bool readStream(stream_t &stream)
{
	size_t result{};
	return stream.read(nullptr, 0, result) || result != 0;
}

bool streamAtEOF(stream_t &stream)
	{ return stream.atEOF(); }
void streamReadSync(stream_t &stream)
	{ stream.readSync(); }
void streamWriteSync(stream_t &stream)
	{ stream.writeSync(); }
void streamDelete(stream_t *stream)
	{ delete stream; }
