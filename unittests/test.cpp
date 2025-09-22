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

namespace unittests {

TEST (HostAPI, write)
{
  const char stdout [] = "stdout\n", stderr [] = "stderr\n";
  EXPECT_EQ (host_write (1, stdout, strlen (stdout)), 0);
  EXPECT_EQ (host_write (1, stderr, strlen (stderr)), 0);
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

}
