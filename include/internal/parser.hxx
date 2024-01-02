// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2018,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

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
