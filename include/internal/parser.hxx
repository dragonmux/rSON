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

#ifndef INTERNAL_PARSER_HXX
#define INTERNAL_PARSER_HXX

#include <memory>
#include <queue>
#include <system_error>
#include "rSON.hxx"

typedef struct JSONParser
{
private:
	stream_t &json;
	char next;
	bool lastWasComma;

	void validateUnicodeSequence(std::queue<char> &result);

public:
	JSONParser(stream_t &toParse);
	void nextChar();
	void skipWhite();
	void match(const char x, const bool skip);
	bool lastTokenComma() const noexcept;
	void lastNoComma() noexcept;
	char currentChar();
	char *literal();
	std::string string();
	size_t number(const bool zeroSpecial, size_t *const decDigits = nullptr);
} JSONParser;

std::unique_ptr<JSONAtom> expression(JSONParser &parser, const bool matchComma = true);
inline size_t length(const char *const str) noexcept { return strlen(str) + 1; }

size_t power10(size_t power);
std::unique_ptr<JSONAtom> object(JSONParser &parser);
std::unique_ptr<JSONAtom> array(JSONParser &parser);
std::unique_ptr<JSONAtom> number(JSONParser &parser);
std::unique_ptr<JSONAtom> literal(JSONParser &parser);

#endif /*INTERNAL_PARSER_HXX*/
