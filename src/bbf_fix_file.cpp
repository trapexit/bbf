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

#include "blkdev.hpp"
#include "captcha.hpp"
#include "errors.hpp"
#include "file.hpp"
#include "filetoblkdev.hpp"
#include "options.hpp"
#include "signals.hpp"

#include <iostream>
#include <utility>

#include <errno.h>
#include <stdint.h>


namespace l
{
  static
  int
  fix_file_loop_core(BlkDev             &blkdev_,
                     char               *buf_,
                     const uint64_t      buflen_,
                     const unsigned int  retries_,
                     const uint64_t      badblock_)
  {
    int rv;
    uint64_t attempts;

    if(signals::signaled_to_exit())
      return -EINTR;

    rv = blkdev_.read(badblock_,1,buf_,buflen_);
    for(attempts = 1; ((attempts <= retries_) && (rv < 0)); attempts++)
      {
        std::cout << "Reading block "
                  << badblock_
                  << " failed (attempt "
                  << attempts << " of " << retries_
                  << "[" << Error::to_string(-rv) << "]: trying again"
                  << std::endl;
        rv = blkdev_.read(badblock_,1,buf_,buflen_);
      }

    if(rv < 0)
      {
        std::cout << "Reading block "
                  << badblock_
                  << " failed ("
                  << attempts << " attempts) "
                  << "[" << Error::to_string(-rv) << "]: using zeros"
                  << std::endl;
        ::memset(buf_,0,buflen_);
      }

    rv = blkdev_.write(badblock_,1,buf_,buflen_);
    for(attempts = 1; ((attempts <= retries_) && (rv < 0)); attempts++)
      {
        std::cout << "Writing block "
                  << badblock_
                  << " failed (attempt "
                  << attempts << " of " << retries_
                  << "[" << Error::to_string(-rv) << "]: trying again"
                  << std::endl;
        rv = blkdev_.write(badblock_,1,buf_,buflen_);
      }

    if(rv < 0)
      std::cout << "Writing block "
                << badblock_
                << " failed ("
                << attempts << " attempts) "
                << "[" << Error::to_string(-rv) << "]"
                << std::endl;

    return 0;
  }


  static
  int
  fix_file_loop(BlkDev                  &blkdev_,
                const File::BlockVector &blockvector_,
                const unsigned int       retries_)
  {
    int rv;
    char *buf;
    uint64_t buflen;

    buflen = blkdev_.logical_block_size();
    buf = new char[buflen];

    for(uint64_t i = 0, ei = blockvector_.size(); i != ei; i++)
      {
        uint64_t        j = blockvector_[i].block;
        const uint64_t ej = blockvector_[i].length + j;

        for(; j != ej; j++)
          {
            rv = l::fix_file_loop_core(blkdev_,buf,buflen,retries_,j);
            if(rv < 0)
              break;
          }
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
  fix_file(const Options &opts_)
  {
    int rv;
    BlkDev blkdev;
    std::string devpath;
    File::BlockVector blockvector;

    rv = File::blocks(opts_.device,blockvector);
    if(rv < 0)
      return AppError::opening_file(-rv,opts_.device);

    devpath = FileToBlkDev::find(opts_.device);
    if(devpath.empty())
      return AppError::opening_device(ENOENT,opts_.device);

    rv = blkdev.open_rdwr(devpath,!opts_.force);
    if(rv < 0)
      return AppError::opening_device(-rv,devpath);
    l::set_blkdev_rwtype(blkdev,opts_.rwtype);

    const std::string captcha = captcha::calculate(blkdev);
    if(opts_.captcha != captcha)
      return AppError::captcha(opts_.captcha,captcha);

    rv = l::fix_file_loop(blkdev,blockvector,opts_.retries);

    rv = blkdev.close();
    if(rv < 0)
      return AppError::closing_device(-rv,opts_.device);

    return AppError::success();
  }
}

namespace bbf
{
  AppError
  fix_file(const Options &opts_)
  {
    return l::fix_file(opts_);
  }
}
