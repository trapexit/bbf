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

#include "badblockfile.hpp"
#include "blkdev.hpp"
#include "captcha.hpp"
#include "errors.hpp"
#include "options.hpp"
#include "signals.hpp"

#include <iostream>
#include <vector>

#include <errno.h>
#include <stdint.h>


namespace l
{
  static
  int
  fix_loop_core(BlkDev             &blkdev_,
                char               *buf_,
                const uint64_t      buflen_,
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
  fix_loop(BlkDev                      &blkdev_,
           const std::vector<uint64_t> &badblocks_,
           const unsigned int           retries_)
  {
    int rv;
    char *buf;
    uint64_t buflen;

    buflen = blkdev_.logical_block_size();
    buf    = new char[buflen];

    for(uint64_t i = 0, ei = badblocks_.size(); i != ei; ++i)
      {
        rv = l::fix_loop_core(blkdev_,buf,buflen,retries_,badblocks_[i]);
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
  fix(const Options &opts_)
  {
    int rv;
    BlkDev blkdev;
    std::string input_file;
    std::vector<uint64_t> badblocks;

    input_file  = opts_.input_file;

    rv = blkdev.open_rdwr(opts_.device,!opts_.force);
    if(rv < 0)
      return AppError::opening_device(-rv,opts_.device);
    l::set_blkdev_rwtype(blkdev,opts_.rwtype);

    const std::string captcha = captcha::calculate(blkdev);
    if(opts_.captcha != captcha)
      return AppError::captcha(opts_.captcha,captcha);

    if(input_file.empty())
      input_file = BadBlockFile::filepath(blkdev);

    rv = BadBlockFile::read(input_file,badblocks);
    if(rv < 0)
      return AppError::reading_badblocks_file(-rv,input_file);
    std::cout << "Imported bad blocks from: " << input_file << std::endl;

    rv = l::fix_loop(blkdev,badblocks,opts_.retries);

    rv = blkdev.close();
    if(rv < 0)
      return AppError::closing_device(-rv,opts_.device);

    return AppError::success();
  }
}

namespace bbf
{
  AppError
  fix(const Options &opts_)
  {
    return l::fix(opts_);
  }
}
