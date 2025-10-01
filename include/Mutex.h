/*
* Nirvana mock library.
*
* This is a part of the Nirvana project.
*
* Author: Igor Popov
*
* Copyright (c) 2025 Igor Popov.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*
* Send comments and/or bug reports to:
*  popov.nirvana@gmail.com
*/
#ifndef NIRVANA_MOCK_MUTEX_H_
#define NIRVANA_MOCK_MUTEX_H_
#pragma once

#include <stdexcept>
#include "export.h"

struct host_Mutex;

NIRVANA_MOCK_EXPORT host_Mutex* host_Mutex_create ();
NIRVANA_MOCK_EXPORT void host_Mutex_destroy (host_Mutex* p);
NIRVANA_MOCK_EXPORT int host_Mutex_lock (host_Mutex* p);
NIRVANA_MOCK_EXPORT int host_Mutex_unlock (host_Mutex* p);

namespace Nirvana {
namespace Mock {

// Drop-in replacement for std::mutex
class mutex
{
public:
	mutex () :
		impl_ (host_Mutex_create ())
	{
    if (!impl_)
      throw std::runtime_error ("Host API error");
  }

	~mutex () noexcept
	{
		host_Mutex_destroy (impl_);
	}

	void lock ()
	{
		if (!host_Mutex_lock (impl_))
      throw std::runtime_error ("Host API error");
	}

	void unlock ()
	{
		if (!host_Mutex_unlock (impl_))
      throw std::runtime_error ("Host API error");
	}

private:
	host_Mutex* impl_;
};

template <class Mutex>
class lock_guard
{
public:
	lock_guard (Mutex& mutex) :
		mutex_ (mutex)
	{
		mutex.lock ();
	}

	~lock_guard ()
	{
		mutex_.unlock ();
	}

private:
	Mutex& mutex_;
};

}
}

#endif
