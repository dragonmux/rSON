// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2017,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#ifndef TEST_HEADER_H
#define TEST_HEADER_H

struct streamTest_t final
{
private:
	stream_t stream;

public:
	streamTest_t() = default;
	~streamTest_t() = default;
	bool read();
	bool write();
	bool atEOF() const;
};

void throwNotImplemented();
void readStream();
void badReadStream();
void writeArray();

#endif /*TEST_HEADER_H*/
