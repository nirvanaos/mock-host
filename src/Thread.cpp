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

#include <assert.h>
#include <system_error>
#include "../include/thread.h"

#ifdef _WIN32

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

struct host_Thread
{
	host_Thread (void (*f)(void *), void *p) :
		function (f), param (p),
		thread (CreateThread (nullptr, 0, thread_proc, this, 0, nullptr))
	{
		if (!thread)
			throw std::system_error (ENOMEM, std::system_category ());
	}

	~host_Thread ()
	{
		assert (!thread);
	}

	int join ()
	{
		if (thread) {
			WaitForSingleObject (thread, INFINITE);
			CloseHandle (thread);
			thread = nullptr;
		}
		return 0;
	}

	static DWORD WINAPI thread_proc (void *p)
	{
		reinterpret_cast<host_Thread *>(p)->function (
				reinterpret_cast<host_Thread *>(p)->param);
		return 0;
	}

	void (*function)(void *);
	void *param;
	HANDLE thread;
};

#else

#include <pthread.h>
#include <signal.h>

struct host_Thread
{
	host_Thread (void (*f)(void *), void *p) :
		function (f), param (p)
	{
		int err = pthread_create (&thread, nullptr, thread_proc, this);
		if (err)
			throw std::system_error (err, std::system_category ());
	}

	~host_Thread ()
	{
		assert (!thread);
	}

	int join ()
	{
		int err = EINVAL;
		if (thread) {
			err = pthread_join (thread, nullptr);
			thread = 0;
		}
		return err;
	}

	static void* thread_proc (void *p)
	{
		reinterpret_cast<host_Thread *>(p)->function (
				reinterpret_cast<host_Thread *>(p)->param);
		return nullptr;
	}

	void (*function)(void *);
	void *param;
	pthread_t thread;
};

#endif

extern int errno_from_host (int err);

NIRVANA_MOCK_EXPORT host_Thread *host_Thread_create (void (*f)(void *), void *p)
{
	try {
		return new host_Thread (f, p);
	} catch (...) {
		return nullptr;
	}
}

NIRVANA_MOCK_EXPORT int host_Thread_join (host_Thread *thread)
{
	int err = EINVAL;
	if (thread) {
		err = thread->join ();
		delete thread;
	}
	
	if (err)
		err = errno_from_host (err);
	return err;
}
