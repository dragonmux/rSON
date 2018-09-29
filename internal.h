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

#include "rSON.h"

namespace rSON
{
	namespace internal
	{
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
