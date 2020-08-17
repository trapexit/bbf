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


#include "fiemap.hpp"
#include "filetoblkdev.hpp"
#include "ioctl.hpp"

#include <string>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


namespace File
{
  typedef struct { uint64_t block; uint64_t length; } Block;
  typedef std::vector<Block> BlockVector;
}

static
int
blocks(const int          fd,
       const int64_t      blocksize,
       File::BlockVector &blockvector)
{
  struct fiemap *fm;

  fm = FIEMap::extent_map(fd);
  if(fm == NULL)
    return -ENOMEM;

  for(int i = 0; i < fm->fm_mapped_extents; i++)
    {
      File::Block block;
      const struct fiemap_extent *fme = &fm->fm_extents[i];

      block.block  = (fme->fe_physical / blocksize);
      block.length = (fme->fe_length   / blocksize);

      blockvector.push_back(block);
    }

  ::free(fm);

  return 0;
}

namespace File
{
  int64_t
  logical_block_size(const std::string &filepath)
  {
    int fd;
    int64_t logical_block_size;
    std::string devpath;

    devpath = FileToBlkDev::find(filepath);
    if(devpath.empty())
      return -ENOENT;

    fd = ::open(devpath.c_str(),O_RDONLY|O_NONBLOCK);
    if(fd == -1)
      return -errno;

    logical_block_size = IOCtl::logical_block_size(fd);

    ::close(fd);

    return logical_block_size;
  }

  int
  blocks(const std::string &filepath,
         BlockVector       &blockvector)
  {
    int fd;
    int rv;
    int64_t logical_block_size;

    logical_block_size = File::logical_block_size(filepath);
    if(logical_block_size < 0)
      return logical_block_size;

    fd = ::open(filepath.c_str(),O_RDONLY);
    if(fd == -1)
      return -errno;

    rv = ::blocks(fd,logical_block_size,blockvector);

    ::close(fd);

    return rv;
  }
}
