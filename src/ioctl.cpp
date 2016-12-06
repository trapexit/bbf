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


#include <errno.h>
#include <inttypes.h>
#include <linux/fiemap.h>
#include <linux/fs.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>

namespace IOCtl
{
  int
  logical_block_size(const int fd)
  {
    int rv;
    int size;

    rv = ::ioctl(fd, BLKSSZGET, &size);
    if(rv == -1)
      return -errno;

    return size;
  }

  int
  physical_block_size(const int fd)
  {
    int rv;
    int size;

    rv = ::ioctl(fd,BLKBSZGET,&size);
    if(rv == -1)
      return -errno;

    return size;
  }

  int64_t
  size_in_bytes(const int fd)
  {
    int rv;

#ifdef BLKGETSIZE64
    {
      int64_t size;

      rv = ::ioctl(fd,BLKGETSIZE64,&size);
      if(rv != -1)
        return size;
    }
#endif

    {
      unsigned long size;

      rv = ::ioctl(fd,BLKGETSIZE,&size);
      if(rv != 0)
        return ((int64_t)size * 512);
    }

    return -errno;
  }

  int64_t
  logical_block_count(const int fd)
  {
    int logical_block_size;
    int64_t size_in_bytes;

    logical_block_size = IOCtl::logical_block_size(fd);
    if(logical_block_size < 0)
      return logical_block_size;

    size_in_bytes = IOCtl::size_in_bytes(fd);
    if(size_in_bytes < 0)
      return size_in_bytes;

    return (size_in_bytes / logical_block_size);
  }

  int64_t
  physical_block_count(const int fd)
  {
    int physical_block_size;
    int64_t size_in_bytes;

    physical_block_size = IOCtl::physical_block_size(fd);
    if(physical_block_size < 0)
      return physical_block_size;

    size_in_bytes = IOCtl::size_in_bytes(fd);
    if(size_in_bytes < 0)
      return size_in_bytes;

    return (size_in_bytes / physical_block_size);
  }

  int
  block_flush(const int fd)
  {
    int rv;

    rv = ::ioctl(fd,BLKFLSBUF,NULL);

    return ((rv == -1) ? -errno : rv);
  }
}
