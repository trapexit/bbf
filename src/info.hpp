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

#ifndef __INFO_HPP__
#define __INFO_HPP__

#include "vectors.hpp"

#include <iostream>

#include <stdint.h>

class InfoPrinter
{
public:
  InfoPrinter();

public:
  void init(const size_t  start_block_,
            const size_t  end_block_,
            U64Vec       *badblocks_);

  void print(const size_t current_block_);

private:
  double  _start_time;
  size_t  _start_block;
  size_t  _end_block;
  U64Vec *_badblocks;
};

namespace Info
{
  void
  print(std::ostream                &os,
        const double                 start_time,
        const double                 current_time,
        const size_t                 start_block,
        const size_t                 end_block,
        const size_t                 current_block,
        const std::vector<uint64_t> &badblocks);

  void
  print(const double                 start_time,
        const double                 current_time,
        const size_t                 start_block,
        const size_t                 end_block,
        const size_t                 current_block,
        const std::vector<uint64_t> &badblocks);

  void
  print(const double                 start_time,
        const size_t                 start_block,
        const size_t                 end_block,
        const size_t                 current_block,
        const std::vector<uint64_t> &badblocks);
}

#endif
