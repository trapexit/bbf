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
#include <vector>

#include "badblockfile.hpp"
#include "blkdev.hpp"
#include "captcha.hpp"
#include "errors.hpp"
#include "options.hpp"
#include "signals.hpp"

static
int
fix_loop_core(BlkDev             &blkdev_,
              char               *buf_,
              const size_t        buflen_,
              const unsigned int  retries_,
              const uint64_t      badblock_)
{
  int rv;
  uint64_t attempts;

  if(signals::signaled_to_exit())
    return -EINTR;

  std::cout << "Reading block " << badblock_ << ' ' << std::flush;

  rv = -1;
  for(attempts = 0; ((attempts <= retries_) && (rv < 0)); attempts++)
    rv = blkdev_.read(badblock_,1,buf_,buflen_);

  if(rv < 0)
    std::cout << "failed [" << Error::to_string(-rv) << "] - using zeros";
  else
    std::cout << "succeeded";
  std::cout << " (" << attempts << " attempts)" << std::endl;

  if(rv < 0)
    memset(buf_,0,buflen_);

  std::cout << "Writing block " << badblock_ << ' ' << std::flush;

  rv = -1;
  for(attempts = 0; ((attempts <= retries_) && (rv < 0)); attempts++)
    rv = blkdev_.write(badblock_,1,buf_,buflen_);

  if(rv < 0)
    std::cout << "failed [" << Error::to_string(-rv) << "]";
  else
    std::cout << "succeeded";
  std::cout << " (" << attempts << " attempts)" << std::endl;

  return 0;
}

static
int
fix_loop(BlkDev                      &blkdev,
         const std::vector<uint64_t> &badblocks,
         const unsigned int           retries)
{
  int rv;
  char *buf;
  size_t buflen;

  buflen = blkdev.logical_block_size();
  buf    = new char[buflen];

  for(uint64_t i = 0, ei = badblocks.size(); i != ei; ++i)
    {
      rv = fix_loop_core(blkdev,buf,buflen,retries,badblocks[i]);
      if(rv < 0)
        break;
    }

  delete[] buf;

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
fix(const Options &opts)
{
  int rv;
  BlkDev blkdev;
  std::vector<uint64_t> badblocks;

  rv = BadBlockFile::read(opts.input_file,badblocks);
  if(rv < 0)
    return AppError::reading_badblocks_file(-rv,opts.input_file);

  rv = blkdev.open_rdwr(opts.device,!opts.force);
  if(rv < 0)
    return AppError::opening_device(-rv,opts.device);

  const std::string captcha = captcha::calculate(blkdev);
  if(opts.captcha != captcha)
    return AppError::captcha(opts.captcha,captcha);

  set_blkdev_rwtype(blkdev,opts.rwtype);

  rv = fix_loop(blkdev,badblocks,opts.retries);

  rv = blkdev.close();
  if(rv < 0)
    return AppError::closing_device(-rv,opts.device);

  return AppError::success();
}

namespace bbf
{
  AppError
  fix(const Options &opts)
  {
    return ::fix(opts);
  }
}
