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


#include "blocktofilemapper.hpp"
#include "fiemap.hpp"
#include "file.hpp"
#include "filetoblkdev.hpp"
#include "ioctl.hpp"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


namespace iter
{
  template<class T>
  T
  next(T i)
  {
    return ++i;
  }
}

static
bool
dot_or_dot_dot(const char *path)
{
  return ((path[0]                    == '.')  &&
          ((path[1] == '.' && path[2] == '\0') ||
           (path[1]                   == '\0')));
}

BlockToFileMapper::BlockToFileMapper()
{

}

static
int
get_blocks(const int                    fd,
           const std::string           &basepath,
           const char                  *filename,
           const int                    blocksize,
           BlockToFileMapper::BlockMap &blockmap)
{
  struct fiemap *fm;

  fm = FIEMap::extent_map(fd);
  if(fm == NULL)
    return -ENOMEM;

  for(int i = 0; i < fm->fm_mapped_extents; i++)
    {
      uint64_t key;
      std::pair<uint64_t,std::string> value;
      const struct fiemap_extent *fme = &fm->fm_extents[i];

      key          = (fme->fe_physical / blocksize);
      value.first  = (fme->fe_length   / blocksize);
      value.second = basepath + '/' + filename;

      blockmap.insert(std::make_pair(key,value));
    }

  ::free(fm);

  return 0;
}

static
bool
scan(const std::string           &basepath,
     const uint64_t               blocksize,
     BlockToFileMapper::BlockMap &blockmap)
{
  int dfd;
  DIR *dir;
  struct dirent *d;

  dir = ::opendir(basepath.c_str());
  if(dir == NULL)
    return false;

  dfd = ::dirfd(dir);
  for(d = ::readdir(dir); d != NULL; d = ::readdir(dir))
    {
      switch(d->d_type)
        {
        case DT_DIR:
          {
            std::string dirpath(basepath);

            if(dot_or_dot_dot(d->d_name))
              break;

            dirpath += '/';
            dirpath += d->d_name;

            scan(dirpath,blocksize,blockmap);
          }
          break;
        case DT_REG:
          {
            int fd;
            int rv;

            fd = ::openat(dfd,d->d_name,O_RDONLY|O_NOFOLLOW);
            if(fd == -1)
              break;

            rv = get_blocks(fd,basepath,d->d_name,blocksize,blockmap);

            ::close(fd);
          }
          break;
        default:
          break;
        }
    }

  ::closedir(dir);

  return true;
}

static
void
compress(BlockToFileMapper::BlockMap &blockmap)
{
  // find contiguous block ranges and merge
  for(BlockToFileMapper::BlockMap::iterator
        i = blockmap.begin();
      i != blockmap.end();
      ++i)
    {
      BlockToFileMapper::BlockMap::iterator curr = i;
      BlockToFileMapper::BlockMap::iterator next = iter::next(i);

      while((next != blockmap.end()) && (curr->second.second == next->second.second))
        {
          if((curr->first + curr->second.first) != next->first)
            break;

          curr->second.first += next->second.first;
          blockmap.erase(next);
          next = iter::next(i);
        }
    }
}

int
BlockToFileMapper::scan(const std::string &basepath)
{
  int rv;
  int64_t blocksize;

  blocksize = File::logical_block_size(basepath);
  if(blocksize < 0)
    return blocksize;

  rv = ::scan(basepath,blocksize,_blockmap);

  ::compress(_blockmap);

  return rv;
}

std::pair<bool,std::string>
BlockToFileMapper::find(const uint64_t block) const
{
  BlockMap::const_iterator iter;


  iter = _blockmap.upper_bound(block);
  if(iter != _blockmap.end())
    --iter;

  if(iter == _blockmap.end())
    return std::make_pair(false,std::string());

  if(block < (iter->first + iter->second.first))
    return std::make_pair(true,iter->second.second);

  return std::make_pair(false,std::string());
}

const
BlockToFileMapper::BlockMap &
BlockToFileMapper::blockmap(void) const
{
  return _blockmap;
}
