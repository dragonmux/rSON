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

bool writeStream(stream_t &stream)
{
	return stream.write(nullptr, 0);
}

bool readStream(stream_t &stream)
{
	size_t result{};
	return stream.read(nullptr, 0, result) || result != 0;
}

bool streamAtEOF(stream_t &stream)
{
	return stream.atEOF();
}
