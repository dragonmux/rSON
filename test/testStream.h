#ifndef TEST_STREAM__H
#define TEST_STREAM__H

bool writeStream(stream_t &stream);
bool readStream(stream_t &stream);
bool streamAtEOF(stream_t &stream);
void streamReadSync(stream_t &stream);
void streamWriteSync(stream_t &stream);
void streamDelete(stream_t *stream);

#endif /*TEST_STREAM__H*/
