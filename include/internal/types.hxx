/*
 * This file is part of rSON
 * Copyright © 2012-2018 Rachel Mant (dx-mon@users.sourceforge.net)
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

#ifndef INTERNAL__H
#define INTERNAL__H

#include <functional>
#include <map>
#include "rSON.hxx"

namespace rSON
{
	namespace internal
	{
		using string_t = std::unique_ptr<char []>;

		struct stringLess_t
		{
			inline bool operator()(const std::unique_ptr<char []> &x, const std::unique_ptr<char []> &y) const
				{ return strcmp(x.get(), y.get()) < 0; }
			inline bool operator()(const std::unique_ptr<char []> &x, const char *const y) const
				{ return strcmp(x.get(), y) < 0; }
			inline bool operator()(const char *const x, const std::unique_ptr<char []> &y) const
				{ return strcmp(x, y.get()) < 0; }
			inline bool operator()(const char *const x, const char *const y) const
				{ return strcmp(x, y) < 0; }
			using is_transparent = typename std::less<>::is_transparent;
		};

		struct object_t final
		{
		private:
			using holder_t = std::map<std::unique_ptr<char []>, jsonAtomPtr_t, stringLess_t>;
			using list_t = std::vector<const char *>;
			holder_t children{};
			list_t mapKeys{};

		public:
			using iter_t = holder_t::iterator;
			using constIter_t = holder_t::const_iterator;

			object_t() = default;
			void clone(const object_t &object);
			void add(const char *const key, jsonAtomPtr_t &&value);
			void del(const char *const key);
			JSONAtom &operator [](const char *const key) const;
			const std::vector<const char *> &keys() const noexcept { return mapKeys; }
			bool exists(const char *const key) const noexcept;
			size_t size() const noexcept { return children.size(); }
			size_t count() const noexcept { return children.size(); }

			iter_t begin() noexcept { return children.begin(); }
			constIter_t begin() const noexcept { return children.begin(); }
			iter_t end() noexcept { return children.end(); }
			constIter_t end() const noexcept { return children.end(); }
		};

		struct array_t final
		{
		private:
			using holder_t = std::vector<jsonAtomPtr_t>;
			holder_t children{};

		public:
			using iter_t = holder_t::iterator;
			using constIter_t = holder_t::const_iterator;

			array_t() = default;
			void clone(const array_t &array);
			void add(jsonAtomPtr_t &&value);
			void del(const size_t key);
			void del(const JSONAtom &value);
			JSONAtom &operator [](const size_t key) const;
			size_t size() const noexcept { return children.size(); }
			size_t count() const noexcept { return children.size(); }
			const JSONAtom *last() const noexcept;

			iter_t begin() noexcept { return children.begin(); }
			constIter_t begin() const noexcept { return children.begin(); }
			iter_t end() noexcept { return children.end(); }
			constIter_t end() const noexcept { return children.end(); }
		};

		template<typename T> inline static void del(void *const object)
		{
			if (object)
				static_cast<T *>(object)->~T();
			operator delete(object);
		}

		template<typename T> struct makeOpaque_ { using uniqueType = opaquePtr_t<T>; };
		template<typename T> struct makeOpaque_<T []> { using arrayType = opaquePtr_t<T []>; };
		template<typename T, size_t N> struct makeOpaque_<T [N]> { struct invalidType { }; };

#if !defined(_MSC_VER) || _MSC_VER >= 1928
		template<typename T, typename... Args> inline typename makeOpaque_<T>::uniqueType makeOpaque(Args &&...args)
		{
			using consT = typename std::remove_const<T>::type;
			return opaquePtr_t<T>(new consT(std::forward<Args>(args)...), del<T>);
		}
#endif

		template<typename T> inline typename makeOpaque_<T>::arrayType makeOpaque(const size_t num)
		{
			using consT = typename std::remove_const<typename std::remove_extent<T>::type>::type;
			return opaquePtr_t<T>(new consT[num](), del<T []>);
		}

		template<typename T, typename... Args> inline typename makeOpaque_<T>::invalidType makeOpaque(Args &&...) noexcept = delete;
	}
}

using namespace rSON;
using namespace rSON::internal;

inline size_t strlen(const std::unique_ptr<char []> &str) noexcept { return strlen(str.get()); }

#endif /*INTERNAL__H*/
