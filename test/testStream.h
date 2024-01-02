// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2019,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#ifndef TEST_STREAM_H
#define TEST_STREAM_H

bool writeStream(stream_t &stream);
bool readStream(stream_t &stream);
bool streamAtEOF(stream_t &stream);
void streamReadSync(stream_t &stream);
void streamWriteSync(stream_t &stream);
void streamDelete(stream_t *stream);

#endif /*TEST_STREAM_H*/
