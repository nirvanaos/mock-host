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
#include "../include/mutex.h"

#ifdef _WIN32

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

struct host_Mutex
{
	host_Mutex ()
	{
		InitializeCriticalSection (&cs);
	}

	~host_Mutex ()
	{
		DeleteCriticalSection (&cs);
	}

	bool lock ()
	{
		EnterCriticalSection (&cs);
    return true;
	}

	bool unlock ()
	{
		LeaveCriticalSection (&cs);
    return true;
	}

	CRITICAL_SECTION cs;
};

#else

#include <pthread.h>

struct host_Mutex
{
	host_Mutex ()
	{
		if (pthread_mutex_init (&mutex, nullptr))
      throw std::runtime_error ("Runtime error");
	}

	~host_Mutex ()
	{
		pthread_mutex_destroy (&mutex);
	}

	bool lock ()
	{
		return pthread_mutex_lock (&mutex) == 0;
	}

	bool unlock ()
	{
		return pthread_mutex_unlock (&mutex) == 0;
	}

	pthread_mutex_t mutex;
};

#endif

NIRVANA_MOCK_EXPORT host_Mutex* host_Mutex_create ()
{
  try {
	  return new host_Mutex;
  } catch (...) {
    return nullptr;
  }
}

NIRVANA_MOCK_EXPORT void host_Mutex_destroy (host_Mutex* p)
{
	delete p;
}

NIRVANA_MOCK_EXPORT int host_Mutex_lock (host_Mutex* p)
{
	return p->lock ();
}

NIRVANA_MOCK_EXPORT int host_Mutex_unlock (host_Mutex* p)
{
	return p->unlock ();
}
