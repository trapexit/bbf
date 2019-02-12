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
#include <stdint.h>

#include <iostream>
#include <utility>

#include "blkdev.hpp"
#include "captcha.hpp"
#include "errors.hpp"
#include "file.hpp"
#include "filetoblkdev.hpp"
#include "options.hpp"
#include "signals.hpp"

static
int
fix_file_loop_core(BlkDev             &blkdev,
                   const uint64_t      logical_block_size,
                   const unsigned int  retries,
                   const uint64_t      badblock)
{
  int rv;
  uint64_t attempts;
  char buf[logical_block_size];

  if(signals::signaled_to_exit())
    return -EINTR;

  rv = blkdev.read(badblock,buf,logical_block_size);
  for(attempts = 1; ((attempts <= retries) && (rv < 0)); attempts++)
    {
      std::cout << "Reading block "
                << badblock
                << " failed (attempt "
                << attempts << " of " << retries
                << "[" << Error::to_string(-rv) << "]: trying again"
                << std::endl;
      rv = blkdev.read(badblock,buf,logical_block_size);
    }

  if(rv < 0)
    {
      std::cout << "Reading block "
                << badblock
                << " failed ("
                << attempts << " attempts) "
                << "[" << Error::to_string(-rv) << "]: using zeros"
                << std::endl;
      memset(buf,0,logical_block_size);
    }

  rv = blkdev.write(badblock,buf,logical_block_size);
  for(attempts = 1; ((attempts <= retries) && (rv < 0)); attempts++)
    {
      std::cout << "Writing block "
                << badblock
                << " failed (attempt "
                << attempts << " of " << retries
                << "[" << Error::to_string(-rv) << "]: trying again"
                << std::endl;
      rv = blkdev.write(badblock,buf,logical_block_size);
    }

  if(rv < 0)
    std::cout << "Writing block "
              << badblock
              << " failed ("
              << attempts << " attempts) "
              << "[" << Error::to_string(-rv) << "]"
              << std::endl;

  return 0;
}


static
int
fix_file_loop(BlkDev                  &blkdev,
              const File::BlockVector &blockvector,
              const unsigned int       retries)
{
  int rv;
  const uint64_t logical_block_size = blkdev.logical_block_size();

  for(size_t i = 0, ei = blockvector.size(); i != ei; i++)
    {
      uint64_t        j = blockvector[i].block;
      const uint64_t ej = blockvector[i].length + j;

      for(; j != ej; j++)
        {
          rv = fix_file_loop_core(blkdev,logical_block_size,retries,j);
          if(rv < 0)
            break;
        }
    }

  return rv;
}

static
void
set_blkdev_rwtype(BlkDev                &blkdev,
                  const Options::RWType  rwtype)
{
  switch(rwtype)
    {
    case Options::ATA:
      blkdev.set_rw_ata();
      break;
    case Options::OS:
      blkdev.set_rw_os();
      break;
    }
}

static
AppError
fix_file(const Options &opts)
{
  int rv;
  BlkDev blkdev;
  std::string devpath;
  File::BlockVector blockvector;

  rv = File::blocks(opts.device,blockvector);
  if(rv < 0)
    return AppError::opening_file(-rv,opts.device);

  devpath = FileToBlkDev::find(opts.device);
  if(devpath.empty())
    return AppError::opening_device(ENOENT,opts.device);

  rv = blkdev.open_rdwr(devpath,!opts.force);
  if(rv < 0)
    return AppError::opening_device(-rv,devpath);

  const std::string captcha = captcha::calculate(blkdev);
  if(opts.captcha != captcha)
    return AppError::captcha(opts.captcha,captcha);

  set_blkdev_rwtype(blkdev,opts.rwtype);

  rv = fix_file_loop(blkdev,blockvector,opts.retries);

  rv = blkdev.close();
  if(rv < 0)
    return AppError::closing_device(-rv,opts.device);

  return AppError::success();
}

namespace bbf
{
  AppError
  fix_file(const Options &opts)
  {
    return ::fix_file(opts);
  }
}
