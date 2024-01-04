// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2018,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileCopyrightText: 2021 Amyspark <amy@amyspark.me>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Modified by Amyspark <amy@amyspark.me>

#ifndef rSON_SOCKET_HXX
#define rSON_SOCKET_HXX

#include <rSON.hxx>

#ifdef _WIN32
#	ifdef rSON_SOCKET_EXPORT_API
#		define rSON_SOCKET_CLS_API __declspec(dllexport)
#	else
#		define rSON_SOCKET_CLS_API __declspec(dllimport)
#	endif
#	define rSON_SOCKET_DEFAULT_VISIBILITY
#	define rSON_SOCKET_API extern rSON_SOCKET_CLS_API
#else
#	define rSON_SOCKET_DEFAULT_VISIBILITY __attribute__ ((visibility("default")))
#	define rSON_SOCKET_CLS_API rSON_SOCKET_DEFAULT_VISIBILITY
#	define rSON_SOCKET_API extern rSON_SOCKET_CLS_API
#endif

#include <substrate/socket>

namespace rSON
{
	struct rpcStream_t : public stream_t
	{
	public:
		using socketType_t = substrate::socketType_t;
		using socket_t = substrate::socket_t;
	private:
		constexpr static const uint32_t bufferLen = 1024;
		socketType_t family;
		socket_t sock;
		std::unique_ptr<char []> buffer;
		uint32_t pos;
		char lastRead;

		void makeBuffer() noexcept;

	protected:
		rpcStream_t(const socketType_t _family, socket_t _sock, std::unique_ptr<char []> _buffer) noexcept :
			family(_family), sock(std::move(_sock)), buffer(std::move(_buffer)), pos(0), lastRead(0) { }
		rpcStream_t() noexcept;

	public:
		rSON_SOCKET_CLS_API rpcStream_t(const socketType_t type);
		rpcStream_t(const rpcStream_t &) = delete;
		rSON_SOCKET_CLS_API rpcStream_t(rpcStream_t &&) = default;
		rSON_SOCKET_CLS_API ~rpcStream_t() noexcept override = default;
		rpcStream_t &operator =(const rpcStream_t &) = delete;
		rSON_SOCKET_CLS_API rpcStream_t &operator =(rpcStream_t &&) = default;

		rSON_SOCKET_CLS_API bool valid() const noexcept;
		// Either call the listen() API OR the connect() - NEVER both for a rpcStream_t instance.
		rSON_SOCKET_CLS_API bool connect(const char *const where, uint16_t port) noexcept;
		rSON_SOCKET_CLS_API bool listen(const char *const where, uint16_t port) noexcept;
		rSON_SOCKET_CLS_API rpcStream_t accept() const noexcept;

		rSON_SOCKET_CLS_API bool read(void *const value, const size_t valueLen, size_t &actualLen) final;
		rSON_SOCKET_CLS_API bool write(const void *const value, const size_t valueLen) final;
		rSON_SOCKET_CLS_API bool atEOF() const noexcept final;
		rSON_SOCKET_CLS_API void readSync() noexcept final;
		rSON_SOCKET_CLS_API void writeSync() noexcept final;
	};
}

#endif /*rSON_SOCKET_HXX*/
