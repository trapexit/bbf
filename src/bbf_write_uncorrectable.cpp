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

#include <iostream>
#include <utility>
#include <vector>

#include <stdint.h>

namespace l
{
  static
  int
  write_pseudo_uncorrectable_loop(BlkDev                      &blkdev_,
                                  const bool                   logging_,
                                  const std::vector<uint64_t> &badblocks_)
  {
    int rv;
    int error;

    error = 0;
    for(uint64_t i = 0, ei = badblocks_.size(); i != ei; ++i)
      {
        rv = blkdev_.write_pseudo_uncorrectable(badblocks_[i],logging_);
        if(rv < 0)
          error = rv;
      }

    return error;
  }

  static
  int
  write_flagged_uncorrectable_loop(BlkDev                      &blkdev_,
                                   const bool                   logging_,
                                   const std::vector<uint64_t> &badblocks_)
  {
    int rv;
    int error;

    error = 0;
    for(uint64_t i = 0, ei = badblocks_.size(); i != ei; ++i)
      {
        rv = blkdev_.write_flagged_uncorrectable(badblocks_[i],logging_);
        if(rv < 0)
          error = rv;
      }

    return error;
  }
}

namespace bbf
{
  AppError
  write_uncorrectable(const Options &opts_)
  {
    int rv;
    BlkDev blkdev;
    std::string captcha;
    std::string input_file;
    std::vector<uint64_t> badblocks;

    input_file = opts_.input_file;

    rv = blkdev.open_rdwr(opts_.device,!opts_.force);
    if(rv < 0)
      return AppError::opening_device(-rv,opts_.device);

    captcha = captcha::calculate(blkdev);
    if(opts_.captcha != captcha)
      return AppError::captcha(opts_.captcha,captcha);

    if(input_file.empty())
      input_file = BadBlockFile::filepath(blkdev);

    rv = BadBlockFile::read(opts_.input_file,badblocks);
    if(rv < 0)
      return AppError::reading_badblocks_file(-rv,opts_.input_file);

    switch(opts_.instruction)
      {
      case Options::WRITE_PSEUDO_UNCORRECTABLE_WL:
        l::write_pseudo_uncorrectable_loop(blkdev,true,badblocks);
        break;
      case Options::WRITE_PSEUDO_UNCORRECTABLE_WOL:
        l::write_pseudo_uncorrectable_loop(blkdev,false,badblocks);
        break;
      case Options::WRITE_FLAGGED_UNCORRECTABLE_WL:
        l::write_flagged_uncorrectable_loop(blkdev,true,badblocks);
        break;
      case Options::WRITE_FLAGGED_UNCORRECTABLE_WOL:
        l::write_flagged_uncorrectable_loop(blkdev,false,badblocks);
        break;
      }

    rv = blkdev.close();
    if(rv < 0)
      return AppError::closing_device(-rv,opts_.device);

    return AppError::success();
  }
}
