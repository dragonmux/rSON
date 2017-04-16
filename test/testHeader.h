#ifndef TEST_HEADER__H
#define TEST_HEADER__H

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

#endif /*TEST_HEADER__H*/
