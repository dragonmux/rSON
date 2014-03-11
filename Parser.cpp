/*
 * This file is part of rSON
 * Copyright © 2012-2013 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include "internal.h"
//#include "String.h"
#include "Memory.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _MSC_VER
#define pow10(x) pow(10.0, (int)x)
#else
#include <unistd.h>
#endif

typedef struct JSONParser
{
private:
	const char *json;
	const char *next;
	const char *jsonEnd;
	bool lastWasComma;

	void validateUnicodeSequence();

public:
	JSONParser(const char *toParse);
	void nextChar();
	void skipWhite();
	void match(char x, bool skip);
	bool lastTokenComma();
	void lastNoComma();
	char currentChar();
	char *literal();
	char *string();
	size_t number(bool zeroSpecial);
	const char *currentPtr() const;
} JSONParser;

JSONAtom *expression(JSONParser *parser, bool matchComma = true);

// Recognise lower-case letters
inline bool isLowerAlpha(char x)
{
	return x >= 'a' && x <= 'z';
}

// Recognise ASCII control characters
inline bool isControl(char x)
{
	return (x >= 0 && x <= 0x1F) || x == 0x7F;
}

// Recognise characters in the file which are deemed in the JSON alphabet
inline bool isAllowedAlpha(char x)
{
	return x != '"' && x != '\\' && isControl(x) == false;
}

// Recognise standard English numbers
inline bool isNumber(char x)
{
	return x >= '0' && x <= '9';
}

// Recognise the beginning of an object
inline bool isObjectBegin(char x)
{
	return x == '{';
}

// Recognise the end of an object
inline bool isObjectEnd(char x)
{
	return x == '}';
}

// Recognise the beginning of an array
inline bool isArrayBegin(char x)
{
	return x == '[';
}

// Recognise the end of an array
inline bool isArrayEnd(char x)
{
	return x == ']';
}

// Recognise a back-slash
inline bool isSlash(char x)
{
	return x == '\\';
}

// Recognise a double-quote
inline bool isQuote(char x)
{
	return x == '"';
}

// Recognise an exponent delimiter
inline bool isExponent(char x)
{
	return x == 'e' || x == 'E';
}

// Recognise a new line character OS agnoistically
inline bool isNewLine(char x)
{
	return x == '\n' || x == '\r';
}

// Recognise whitespace
inline bool isWhiteSpace(char x)
{
	return x == ' ' || x == '\t' || isNewLine(x);
}

// Recognise a hexadecimal digit
inline bool isHex(char x)
{
	return (x >= '0' && x <= '9') ||
		(x >= 'A' && x <= 'F') ||
		(x >= 'a' && x <= 'f');
}

JSONParser::JSONParser(const char *toParse)
{
	json = toParse;
	next = json;
	jsonEnd = json + strlen(json);
	lastWasComma = false;
}

void JSONParser::nextChar()
{
	if (next == jsonEnd)
		throw JSONParserError(JSON_PARSER_EOF);
	next++;
}

// This function intentionally ignores EOF to prevent the
// parser from exiting via exception when it sees the final } or ]
void JSONParser::skipWhite()
{
	try
	{
		while (isWhiteSpace(currentChar()))
			nextChar();
	}
	catch (JSONParserError &err)
	{
	}
}

// Match the current character with x, and skip whitespace if skip == true.
// Throws an exception if x and the current character do not match.
void JSONParser::match(char x, bool skip)
{
	if (currentChar() == x)
	{
		if (x == ',')
			lastWasComma = true;
		else
			lastWasComma = false;
		nextChar();
		if (skip)
			skipWhite();
	}
	else
		throw JSONParserError(JSON_PARSER_BAD_JSON);
}

bool JSONParser::lastTokenComma()
{
	return lastWasComma;
}

void JSONParser::lastNoComma()
{
	lastWasComma = false;
}

char JSONParser::currentChar()
{
	char ret = *next;
	if (next == jsonEnd)
		throw JSONParserError(JSON_PARSER_EOF);
	return ret;
}

// Parses an alpha-numeric literal
char *JSONParser::literal()
{
	size_t len;
	char *ret;
	const char *start = next;

	if (isLowerAlpha(currentChar()) == false)
		throw JSONParserError(JSON_PARSER_BAD_JSON);
	while (isLowerAlpha(currentChar()))
		nextChar();

	len = (size_t)(next - start) + 1;
	ret = new char[len]();
	memcpy(ret, start, len - 1);
	ret[len - 1] = 0;
	skipWhite();

	return ret;
}

// Verifies a \u sequence
void JSONParser::validateUnicodeSequence()
{
	char len = 0;
	for (len = 0; len < 4 && isHex(next[1]); len++)
		nextChar();
	if (len != 4)
		throw JSONParserError(JSON_PARSER_BAD_JSON);
}

// Parses a string per the JSON string rules
char *JSONParser::string()
{
	size_t len;
	char *str;
	const char *start, *end;
	bool slash = false;

	match('"', false);
	start = next;

	while (isQuote(currentChar()) == false || slash == true)
	{
		if (slash)
		{
			switch (currentChar())
			{
				case '"':
				case '\\':
				case '/':
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':
					break;
				case 'u':
					validateUnicodeSequence();
					// Make sure there are 4 hex characters here
					break;
				default:
					throw JSONParserError(JSON_PARSER_BAD_JSON);
			}
			slash = false;
		}
		else
		{
			slash = isSlash(currentChar());
			if (slash == false && isAllowedAlpha(currentChar()) == false)
				throw JSONParserError(JSON_PARSER_BAD_JSON);
		}
		nextChar();
	}

	end = next;
	match('"', true);

	len = (size_t)(end - start) + 1;
	str = new char[len]();
	memcpy(str, start, len - 1);
	str[len - 1] = 0;
	return str;
}

// Parses a positive natural number
size_t JSONParser::number(bool zeroSpecial)
{
	size_t num = 0;

	if (isNumber(currentChar()) == false)
		throw JSONParserError(JSON_PARSER_BAD_JSON);

	if (zeroSpecial && currentChar() == '0')
	{
		nextChar();
		if (isNumber(currentChar()))
			throw JSONParserError(JSON_PARSER_BAD_JSON);
		return 0;
	}

	while (isNumber(currentChar()))
	{
		num *= 10;
		num += currentChar() - '0';
		nextChar();
	}
	return num;
}

const char *JSONParser::currentPtr() const
{
	return next;
}

// Parses an object
JSONAtom *object(JSONParser *parser)
{
	JSONObject *object = new JSONObject();
	try
	{
		parser->match('{', true);
		while (isObjectEnd(parser->currentChar()) == false)
		{
			char *key = parser->string();
			parser->match(':', true);
			object->add(key, expression(parser));
		}
		if (parser->lastTokenComma())
			throw JSONParserError(JSON_PARSER_BAD_JSON);
		parser->match('}', true);
	}
	catch (JSONParserError &err)
	{
		delete object;
		throw;
	}
	return object;
}

// Parses an array
JSONAtom *array(JSONParser *parser)
{
	JSONArray *array = new JSONArray();
	try
	{
		parser->match('[', true);
		while (isArrayEnd(parser->currentChar()) == false)
			array->add(expression(parser));
		if (parser->lastTokenComma())
			throw JSONParserError(JSON_PARSER_BAD_JSON);
		parser->match(']', true);
	}
	catch (JSONParserError &err)
	{
		delete array;
		throw;
	}
	return array;
}

// Raise 10 to the power of power.
// This is intentionally limited to the positive natural numbers.
size_t power10(size_t power)
{
	size_t i, ret = 1;
	for (i = 0; i < power; i++)
		ret *= 10;
	return ret;
}

// Parses a JSON number, using the positive natural parser.
JSONAtom *number(JSONParser *parser)
{
	bool sign = false, mulSign = false;
	int integer = 0;
	size_t decimal = 0, decDigits = 0, multiplier = 0;
	bool decimalValid = false;

	if (parser->currentChar() == '-')
	{
		parser->match('-', false);
		sign = true;
	}
	integer = parser->number(true);
	if (parser->currentChar() == '.')
	{
		parser->match('.', false);
		const char * const start = parser->currentPtr();
		decimal = parser->number(false);
		decDigits = parser->currentPtr() - start;
		decimalValid = true;
	}
	if (isExponent(parser->currentChar()))
	{
		parser->nextChar();
		if (parser->currentChar() == '-')
		{
			parser->match('-', false);
			mulSign = true;
		}
		else if (parser->currentChar() == '+')
			parser->match('+', false);
		multiplier = parser->number(true);
	}

	if (decimalValid == false)
	{
		int mul = power10(multiplier);
		if (mulSign)
			integer /= mul;
		else
			integer *= mul;
		if (sign)
			return new JSONInt(-integer);
		return new JSONInt(integer);
	}
	else
	{
		size_t mul = pow10(multiplier);
		double num = decimal / pow10(decDigits);
		num += integer;
		if (mulSign)
			num /= mul;
		else
			num *= mul;
		if (sign)
			return new JSONFloat(-num);
		return new JSONFloat(num);
	}
}

// Parses the literals "true", "false" and "null"
JSONAtom *literal(JSONParser *parser)
{
	JSONAtom *ret = NULL;
	char *lit = parser->literal();

	if (strcmp(lit, "true") == 0)
		ret = new JSONBool(true);
	else if (strcmp(lit, "false") == 0)
		ret = new JSONBool(false);
	else if (strcmp(lit, "null") == 0)
		ret = new JSONNull();
	delete [] lit;

	if (ret == NULL)
		throw JSONParserError(JSON_PARSER_BAD_JSON);
	return ret;
}

// Parses an expression of some sort
JSONAtom *expression(JSONParser *parser, bool matchComma)
{
	JSONAtom *atom = NULL;
	switch (parser->currentChar())
	{
		case '{':
			atom = object(parser);
			break;
		case '[':
			atom = array(parser);
			break;
		case '"':
			atom = new JSONString(parser->string());
			break;
	}

	if (atom == NULL)
	{
		if (isNumber(parser->currentChar()))
			atom = number(parser);
		else
			atom = literal(parser);
	}

	if (matchComma && isObjectEnd(parser->currentChar()) == false && isArrayEnd(parser->currentChar()) == false)
		parser->match(',', true);
	else
		parser->lastNoComma();
	return atom;
}

// The parser entry point
// This verifies the first character in the string to parse is the beginning of either an array or an object
// It then performs a try-catch in which expression() is invoked. if an exception is thrown or needs to be thrown,
// the parser object this temporarily creates is cleaned up before the exception is (re)thrown.
// If everything went OK, this then cleans up the parser object and returns the resulting JSONAtom tree.
JSONAtom *rSON::parseJSON(const char *json)
{
	JSONAtom *ret = NULL;
	JSONParser *parser;
	if (json == NULL)
		return NULL;
	parser = new JSONParser(json);

	if (isObjectBegin(parser->currentChar()) || isArrayBegin(parser->currentChar()))
	{
		try
		{
			ret = expression(parser, false);
		}
		catch (JSONParserError &err)
		{
			delete parser;
			throw;
		}
	}
	else
	{
		delete parser;
		throw JSONParserError(JSON_PARSER_BAD_JSON);
	}

	delete parser;
	return ret;
}

// This is the file-wide alternative for the above entry point.
// This automates the process of reading a file and parsing it to produce the JSONAtom *
// so that application code only has to call this rather than writing it's own file IO routines
JSONAtom *rSON::parseJSONFile(const char *file)
{
	struct stat fileStat;
	JSONAtom *ret;
	int jsonFD;
	char *json = NULL;

	jsonFD = open(file, O_RDONLY | O_EXCL | O_NOCTTY);
	if (jsonFD == -1)
		throw JSONParserError(JSON_PARSER_BAD_FILE);

	fstat(jsonFD, &fileStat);
	json = new char[fileStat.st_size]();
	if ((read(jsonFD, json, fileStat.st_size) | close(jsonFD)) == -1)
	{
		delete [] json;
		throw JSONParserError(JSON_PARSER_BAD_FILE);
	}

	try
	{
		ret = rSON::parseJSON(json);
	}
	catch (...)
	{
		delete [] json;
		throw;
	}

	delete [] json;
	return ret;
}
