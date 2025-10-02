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
#ifndef NIRVANA_MOCK_THREAD_H_
#define NIRVANA_MOCK_THREAD_H_
#pragma once

#include <errno.h>

#include <functional>
#include <system_error>
#include <type_traits>

#include "export.h"

struct host_Thread;

NIRVANA_MOCK_EXPORT void* host_allocate (size_t size, size_t align);
NIRVANA_MOCK_EXPORT void host_release (void* p);
NIRVANA_MOCK_EXPORT host_Thread* host_Thread_create (void (*)(void*), void*);
NIRVANA_MOCK_EXPORT int host_Thread_join (host_Thread*);
NIRVANA_MOCK_EXPORT unsigned int host_hardware_concurrency ();

namespace Nirvana {
namespace Mock {

/// Drop-in replacement for std::thread
class thread
{
public:
  thread () noexcept :
    impl_ (nullptr)
  {}

  thread (thread&& src) :
    impl_ (src.impl_)
  {
    src.impl_ = nullptr;
  }

  template <class F, class ... Args>
  thread (F&& f, Args&& ... args) :
    thread (std::bind (std::forward <F> (f), std::forward <Args> (args)...))
  {}

  ~thread ()
  {
		if (impl_)
			std::terminate ();
  }

	thread& operator = (thread&& rhs) noexcept
	{
		if (impl_)
			std::terminate ();
		impl_ = rhs.impl_;
		rhs.impl_ = nullptr;
		return *this;
	}

  bool joinable () const noexcept
  {
    return impl_ != nullptr;
  }

  void join ()
  {
    if (impl_) {
			ImplBase* impl = impl_;
			impl_ = nullptr;
			int err = host_Thread_join (impl->host_thread);
			std::exception_ptr exception = std::move (impl->exception);
      delete impl;
			if (err)
        throw std::system_error (err, std::system_category ());
      if (exception)
        std::rethrow_exception (exception);
    } else
			throw std::invalid_argument ("Not joinable thread");
  }

	void swap (thread& other) noexcept
	{
		std::swap (impl_, other.impl_);
	}

  static unsigned int hardware_concurrency () noexcept
  {
    return host_hardware_concurrency ();
  }

private:
	template <class F>
	thread (F&& f) :
		impl_ (new Impl <F> (std::move (f)))
	{
		if (!(impl_->host_thread = host_Thread_create (thread_proc, impl_))) {
			delete impl_;
     	throw std::system_error (ENOMEM, std::system_category ());
		}
	}

	struct ImplBase
	{
		ImplBase () :
			host_thread (nullptr)
		{}

		virtual ~ImplBase ()
		{}

		virtual void run () = 0;

		std::exception_ptr exception;
		host_Thread* host_thread;
	};

  static void thread_proc (void* param) noexcept
  {
		ImplBase* impl = reinterpret_cast <ImplBase*> (param);
    try {
      impl->run ();
    } catch (...) {
      impl->exception = std::current_exception ();
    }
  }

private:
	template <class F>
	struct Impl : ImplBase
	{
		void* operator new (size_t cb)
		{
			return host_allocate (cb, alignof (Impl));
		}

		void operator delete (void* p) noexcept
		{
			host_release (p);
		}

		Impl (F&& f) :
			function (std::move (f))
		{}

		void run () override
		{
			function ();
		}

		F function;
	};

  ImplBase* impl_;
};

}
}

#endif
