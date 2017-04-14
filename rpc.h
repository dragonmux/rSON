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

#ifndef rSON_RPC__HXX
#define rSON_RPC__HXX

#include <unistd.h>
#include "rSON.h"

namespace rSON
{
	struct socket_t final
	{
	private:
		int32_t socket;

		int32_t release() noexcept
		{
			const int32_t s = socket;
			socket = -1;
			return s;
		}

		void reset(int32_t s = -1) noexcept
		{
			if (socket != -1)
				close(socket);
			socket = s;
		}

	public:
		constexpr socket_t() noexcept : socket(-1) { }
		socket_t(int32_t s) : socket(s) { }
		socket_t(const socket_t &) = delete;
		socket_t(socket_t &&s) noexcept : socket(s.release()) { }
		~socket_t() noexcept { reset(); }

		socket_t &operator =(const socket_t &) = delete;
		void operator =(socket_t &&s) noexcept { reset(s.release()); }
		operator int32_t() const noexcept { return socket; }
		bool operator ==(const int32_t s) const noexcept { return socket == s; }
		void swap(socket_t &s) noexcept { std::swap(socket, s.socket); }
	};

	struct rSON_CLS_API rpcStream_t final : public stream_t
	{
	private:
		int fd;
		size_t length;
		bool eof;
		int32_t mode;

	public:
		rpcStream_t(const char *const fileName, const int32_t mode);
		rpcStream_t(const rpcStream_t &) = delete;
		rpcStream_t(rpcStream_t &&) = default;
		~rpcStream_t() noexcept final override;
		rpcStream_t &operator =(const rpcStream_t &) = delete;
		rpcStream_t &operator =(rpcStream_t &&) = default;

		bool read(void *const value, const size_t valueLen, size_t &actualLen) final override;
		bool write(const void *const value, const size_t valueLen) final override;
		bool atEOF() const noexcept final override { return eof; }
	};
}

#endif /*rSON_RPC__HXX*/