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

#include "rSON.h"

struct sockaddr;

namespace rSON
{
	struct socket_t final
	{
	private:
		int32_t socket;

		bool bind(const void *const addr, const size_t len) const noexcept;
		bool connect(const void *const addr, const size_t len) const noexcept;

	public:
		constexpr socket_t() noexcept : socket(-1) { }
		constexpr socket_t(const int32_t s) noexcept : socket(s) { }
		socket_t(const int family, const int type, const int protocol = 0) noexcept;
		socket_t(const socket_t &) = delete;
		socket_t(socket_t &&s) noexcept : socket(-1) { swap(s); }
		~socket_t() noexcept;

		socket_t &operator =(const socket_t &) = delete;
		void operator =(socket_t &&s) noexcept { swap(s); }
		operator int32_t() const noexcept { return socket; }
		bool operator ==(const int32_t s) const noexcept { return socket == s; }
		bool operator !=(const int32_t s) const noexcept { return socket != s; }
		void swap(socket_t &s) noexcept { std::swap(socket, s.socket); }

		template<typename T> bool bind(const T &addr) const noexcept { return bind(static_cast<const void *>(&addr), sizeof(T)); }
		template<typename T> bool connect(const T &addr) const noexcept { return connect(static_cast<const void *>(&addr), sizeof(T)); }
		bool listen(const int32_t queueLength) const noexcept;
		socket_t accept(sockaddr *peerAddr = nullptr, socklen_t *peerAddrLen = nullptr) const noexcept;
		ssize_t write(const void *const bufferPtr, const size_t len) const noexcept;
		ssize_t read(void *const bufferPtr, const size_t len) const noexcept;
	};

	struct rSON_CLS_API rpcStream_t : public stream_t
	{
	private:
		const int family;
		socket_t sock;

	protected:
		rpcStream_t(const int _family, const int32_t _sock) noexcept :
			family(_family), sock(_sock) { }
		rpcStream_t() noexcept;

	public:
		rpcStream_t(const bool ipv6/* TODO: Implement a transport dispatch */);
		rpcStream_t(const rpcStream_t &) = delete;
		rpcStream_t(rpcStream_t &&) = default;
		~rpcStream_t() noexcept override = default;
		rpcStream_t &operator =(const rpcStream_t &) = delete;
		rpcStream_t &operator =(rpcStream_t &&) = default;

		bool valid() const noexcept;
		// Either call the listen() API OR the connect() - NEVER both for a rpcStream_t instance.
		bool connect(const char *const where, uint16_t port) const noexcept;
		bool listen(const char *const where, uint16_t port) const noexcept;
		rpcStream_t accept() const noexcept;

		bool read(void *const value, const size_t valueLen, size_t &actualLen) final override;
		bool write(const void *const value, const size_t valueLen) final override;
		bool atEOF() const noexcept final override { return false; }
	};
}

#endif /*rSON_RPC__HXX*/
