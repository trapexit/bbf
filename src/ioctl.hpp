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

#ifndef __IOCTL_HPP__
#define __IOCTL_HPP__

#include <stdint.h>
#include <linux/fiemap.h>

namespace IOCtl
{
  int      logical_block_size(const int fd);
  int      physical_block_size(const int fd);

  int64_t size_in_bytes(const int fd);
  int64_t logical_block_count(const int fd);
  int64_t physical_block_count(const int fd);

  int            extent_map_count(const int fd);
  struct fiemap *extent_map(const int fd);

  int block_flush(const int fd);
}

#endif
