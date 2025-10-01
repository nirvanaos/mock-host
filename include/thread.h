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
#include <memory>

#include "export.h"

struct host_Thread;

NIRVANA_MOCK_EXPORT host_Thread* host_Thread_create (void (*)(void*), void*);
NIRVANA_MOCK_EXPORT int host_Thread_join (host_Thread*);

NIRVANA_MOCK_EXPORT void host_abort ();
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
		function_ (std::move (src.function_)),
		exception_ (std::move (src.exception_)),
    impl_ (src.impl_)
  {
    src.impl_ = nullptr;
  }

  template <class F, class ... Args>
  thread (F&& f, Args&& ... args) :
    function_ (std::bind (std::forward <F> (f), std::forward <Args> (args)...)),
    impl_ (host_Thread_create (thread_proc, this))
  {
    if (!impl_)
     	throw std::system_error (ENOMEM, std::system_category ());
  }

  ~thread ()
  {
		if (impl_)
			host_abort ();
  }

  bool joinable () const noexcept
  {
    return impl_ != nullptr;
  }

  void join ()
  {
    if (impl_) {
			int err = host_Thread_join (impl_);
			if (err)
        throw std::system_error (err, std::system_category ());
      impl_ = nullptr;
      if (exception_)
        std::rethrow_exception (exception_);
    }
  }

  static unsigned int hardware_concurrency () noexcept
  {
    return host_hardware_concurrency ();
  }

private:
  static void thread_proc (void* param) noexcept
  {
    try {
      (reinterpret_cast <thread*> (param)->function_) ();
    } catch (...) {
      reinterpret_cast <thread*> (param)->exception_ = std::current_exception ();
    }
  }

private:
  std::function <void()> function_;
  std::exception_ptr exception_;
  host_Thread* impl_;
};

}
}

#endif
