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

#include <linux/fiemap.h>
#include <linux/fs.h>
#include <stdlib.h>
#include <sys/ioctl.h>

namespace FIEMap
{
  int
  extent_map_count(const int fd)
  {
    int rv;
    struct fiemap fm = {0};

    fm.fm_length = FIEMAP_MAX_OFFSET;

    rv = ::ioctl(fd,FS_IOC_FIEMAP,&fm);
    if(rv == -1)
      return -1;

    return fm.fm_mapped_extents;
  }

  struct fiemap *
  extent_map(const int fd)
  {
    int rv;
    int extent_count;
    int extent_size;
    struct fiemap *fm;

    extent_count = FIEMap::extent_map_count(fd);
    if(extent_count == -1)
      return NULL;

    extent_size = (sizeof(struct fiemap_extent) * extent_count);

    fm = (struct fiemap*)calloc(1,(sizeof(struct fiemap) + extent_size));
    if(fm == NULL)
      return NULL;

    fm->fm_length       = FIEMAP_MAX_OFFSET;
    fm->fm_extent_count = extent_count;

    rv = ::ioctl(fd,FS_IOC_FIEMAP,fm);
    if(rv == -1)
      {
        ::free(fm);
        return NULL;
      }

    return fm;
  }
}
