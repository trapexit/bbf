/*
  ISC License

  Copyright (c) 2016, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <iostream>
#include <utility>

#include "blocktofilemapper.hpp"
#include "errors.hpp"
#include "options.hpp"

std::ostream &
operator<<(std::ostream                                      &os,
           const BlockToFileMapper::BlockMap::const_iterator &i)
{
  os << i->first << " - "
     << (i->first + i->second.first - 1)
     << " "
     << i->second.second;
}

std::ostream &
operator<<(std::ostream                      &os,
           const BlockToFileMapper::BlockMap &bm)
{
  for(BlockToFileMapper::BlockMap::const_iterator
        i = bm.begin(), ei = bm.end();
      i != ei;
      ++i)
    {
      os << i << std::endl;
    }
}

namespace bbf
{
  AppError
  dump_files(const Options &opts)
  {
    int rv;
    BlockToFileMapper b2fm;

    rv = b2fm.scan(opts.device);
    if(rv < 0)
      return AppError::opening_device(-rv,opts.device);

    std::cout << b2fm.blockmap();

    return AppError::success();
  }
}
