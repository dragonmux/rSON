/*
 * This file is part of rSON
 * Copyright © 2012-2019 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <stdio.h>
#include <ctype.h>
#include "internal/types.hxx"

uint8_t hex2int(char c)
{
	auto x{static_cast<char>(toupper(c))};
	if ((x >= '0' && x <= '9') || (x >= 'A' && x <= 'F'))
	{
		x -= 0x30; // '0' == 0x30
		if (x >= 10) // 'A' == 0x41, 0x41 - 0x30 = 0x11, 0x11 - 0x07 = 0x0A;
			x -= 0x07;
		return x;
	}
	else
		throw JSONParserError(JSON_PARSER_BAD_JSON);
}

void writeValue(const uint8_t *&readPos, uint8_t *&writePos, const char value)
{
	*writePos = value;
	++readPos;
	++writePos;
}

void parseUnicode(const uint8_t *&readPos, uint8_t *&writePos)
{
	uint8_t i;
	uint16_t charVal = 0;
	for (i = 1; i < 5; i++)
	{
		charVal <<= 4;
		charVal += hex2int(readPos[i]);
	}
	readPos += 5;
	if (charVal == 0x0000)
	{
		writePos[0] = 0xC0;
		writePos[1] = 0x80;
		writePos += 2;
	}
	else if (charVal <= 0x007F)
	{
		writePos[0] = charVal & 0x7F;
		writePos++;
	}
	else if (charVal <= 0x07FF)
	{
		writePos[1] = 0x80 | (charVal & 0x3F);
		charVal >>= 6;
		writePos[0] = 0xC0 | (charVal & 0x1F);
		writePos += 2;
	}
	else
	{
		writePos[2] = 0x80 | (charVal & 0x3F);
		charVal >>= 6;
		writePos[1] = 0x80 | (charVal & 0x3F);
		charVal >>= 6;
		writePos[0] = 0xE0 | (charVal & 0x0F);
		writePos += 3;
	}
}

JSONString::JSONString(char *strValue) : JSONAtom(JSON_TYPE_STRING), value(strValue)
{
	const uint8_t *readPos = (uint8_t *)strValue;
	uint8_t *writePos = (uint8_t *)strValue;
	bool slash = false;

	while (*readPos != 0)
	{
		if (!slash && *readPos == '\\')
		{
			slash = true;
			readPos++;
		}
		else
		{
			if (slash)
			{
				switch (*readPos)
				{
					case 'u':
					{
						parseUnicode(readPos, writePos);
						break;
					}
					case 'n':
						writeValue(readPos, writePos, '\n');
						break;
					case 'r':
						writeValue(readPos, writePos, '\r');
						break;
					case 't':
						writeValue(readPos, writePos, '\t');
						break;
					case '"':
					case '\\':
					case '/':
						writeValue(readPos, writePos, *readPos);
						break;
					case 'b':
						writeValue(readPos, writePos, '\x08');
						break;
					case 'f':
						writeValue(readPos, writePos, '\x0C');
						break;
				}
				slash = false;
			}
			else
			{
				if (writePos != readPos)
					*writePos = *readPos;
				writePos++;
				readPos++;
			}
		}
	}
	writePos[0] = 0;
}

JSONString::~JSONString()
{
	delete [] value;
}

JSONString::operator const char *() const
{
	return value;
}

void JSONString::set(char *strValue)
{
	delete [] value;
	value = strValue;
}

size_t JSONString::len() const
{
	// Note, this works specifically because we surrogate pair encode the NULL byte in the decoder.
	// If the caller needs their string surrogate decoded, they should ask for the string raw value,
	// this, and in a seperate buffer, decode the string fully.
	return strlen(value);
}
