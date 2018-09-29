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

#include <functional>
#include "rSON.h"

namespace rSON
{
	namespace internal
	{
		using string_t = std::unique_ptr<char []>;

		struct stringLess_t
		{
			using is_transparent = typename std::less<>::is_transparent;

			inline bool operator()(const string_t &x, const string_t &y) const
				{ return strcmp(x.get(), y.get()) < 0; }

			inline bool operator()(const string_t &x, const char *const y) const
				{ return strcmp(x.get(), y) < 0; }

			inline bool operator()(const char *const x, const string_t &y) const
				{ return strcmp(x, y.get()) < 0; }

			inline bool operator()(const char *const x, const char *const y) const
				{ return strcmp(x, y) < 0; }
		};

		struct object_t final
		{
		private:
			using map_t = std::map<string_t, std::unique_ptr<JSONAtom>, stringLess_t>;
			using list_t = std::vector<const char *>;

			map_t children;
			list_t mapKeys;

		public:
			object_t() : children{}, mapKeys{} { }
			void clone(const object_t &object);
			void add(const char *const key, std::unique_ptr<JSONAtom> &&value);
			void del(const char *const key);
			JSONAtom &operator [](const char *const key) const;
			const std::vector<const char *> &keys() const noexcept { return mapKeys; }
			bool exists(const char *const key) const noexcept;
			size_t size() const noexcept { return children.size(); }
			size_t count() const noexcept { return size(); }

			map_t::iterator begin() noexcept { return children.begin(); }
			map_t::const_iterator begin() const noexcept { return children.begin(); }
			map_t::iterator end() noexcept { return children.end(); }
			map_t::const_iterator end() const noexcept { return children.end(); }
		};

		template<typename T> struct makeManaged_ { using uniqueType = managedPtr_t<T>; };
		template<typename T> struct makeManaged_<T []> { using arrayType = managedPtr_t<T []>; };
		template<typename T, size_t N> struct makeManaged_<T [N]> { struct invalidType { }; };

		template<typename T, typename... Args> inline typename makeManaged_<T>::uniqueType makeManaged(Args &&...args)
		{
			using consT = typename std::remove_const<T>::type;
			return managedPtr_t<T>(new consT(std::forward<Args>(args)...));
		}

		template<typename T> inline typename makeManaged_<T>::arrayType makeManaged(const size_t num)
		{
			using consT = typename std::remove_const<typename std::remove_extent<T>::type>::type;
			return managedPtr_t<T>(new consT[num]());
		}

		template<typename T, typename... Args> inline typename makeManaged_<T>::invalidType makeManaged(Args &&...) noexcept = delete;
	}
}

using namespace rSON;
using namespace rSON::internal;

inline size_t strlen(const string_t &str) noexcept { return strlen(str.get()); }
