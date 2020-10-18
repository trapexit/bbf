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

std::ostream&
operator<<(std::ostream                                      &os_,
           const BlockToFileMapper::BlockMap::const_iterator &i_)
{
  os_ << i_->first << " - "
      << (i_->first + i_->second.first - 1)
      << " "
      << i_->second.second;
  return os_;
}

std::ostream&
operator<<(std::ostream                      &os_,
           const BlockToFileMapper::BlockMap &bm_)
{
  for(BlockToFileMapper::BlockMap::const_iterator
        i = bm_.begin(), ei = bm_.end();
      i != ei;
      ++i)
    {
      os_ << i << std::endl;
    }

  return os_;
}

namespace bbf
{
  AppError
  dump_files(const Options &opts_)
  {
    int rv;
    BlockToFileMapper b2fm;

    rv = b2fm.scan(opts_.device);
    if(rv < 0)
      return AppError::opening_device(-rv,opts_.device);

    std::cout << b2fm.blockmap();

    return AppError::success();
  }
}
