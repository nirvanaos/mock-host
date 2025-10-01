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
#include <gtest/gtest.h>
#include "../include/HostAPI.h"
#include "../include/thread.h"

using Nirvana::Mock::thread;

namespace unittests {

TEST (HostAPI, write)
{
  const char t1 [] = "stdout\n";
  const char t2 [] = "stderr\n";
  EXPECT_EQ (host_write (1, t1, strlen (t1)), 0);
  EXPECT_EQ (host_write (2, t2, strlen (t2)), 0);
}

TEST (HostAPI, isatty)
{
  EXPECT_TRUE (host_isatty (0));
  EXPECT_TRUE (host_isatty (1));
  EXPECT_TRUE (host_isatty (2));
}

TEST (HostAPI, seek)
{
  uint64_t pos;
  EXPECT_TRUE (host_seek (0, 0, SEEK_CUR, pos));
  EXPECT_TRUE (host_seek (1, 0, SEEK_CUR, pos));
  EXPECT_TRUE (host_seek (2, 0, SEEK_CUR, pos));
}

class ThreadTest
{
public:
  ThreadTest () :
    passed_ (false)
  {}

  void run ()
  {
    passed_ = true;
  }

  bool passed () const
  {
    return passed_;
  }

private:
  bool passed_;
};

void thread_test_func (bool& passed)
{
  passed = true;
}

TEST (HostAPI, thread)
{
  {
    ThreadTest test;
    thread thr (&ThreadTest::run, std::ref (test));
    thr.join ();
    EXPECT_TRUE (test.passed ());
  }
  {
    ThreadTest test;
    thread thr (&ThreadTest::run, &test);
    thr.join ();
    EXPECT_TRUE (test.passed ());
  }
  {
    bool passed = false;
    thread thr (thread_test_func, std::ref (passed));
    thr.join ();
    EXPECT_TRUE (passed);
  }
}

TEST (HostAPI, hardware_concurrency)
{
	ASSERT_NE (thread::hardware_concurrency (), 0);
}

}
