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

#include <stdint.h>

#include <iostream>
#include <utility>
#include <vector>

#include "badblockfile.hpp"
#include "blkdev.hpp"
#include "captcha.hpp"
#include "errors.hpp"
#include "options.hpp"

int
write_uncorrectable_loop(BlkDev                      &blkdev,
                         const std::vector<uint64_t> &badblocks)
{
  int error;

  error = 0;
  for(size_t i = 0, ei = badblocks.size(); i != ei; ++i)
    {
      int tmprv;
      const uint64_t badblock = badblocks[i];

      tmprv = blkdev.write_flagged_uncorrectable(badblock);
      if(tmprv < 0)
        error = tmprv;
    }

  return error;
}

namespace bbf
{
  AppError
  write_uncorrectable(const Options &opts)
  {
    int rv;
    BlkDev blkdev;
    std::string captcha;
    std::string input_file;
    std::vector<uint64_t> badblocks;

    input_file = opts.input_file;

    rv = blkdev.open_rdwr(opts.device);
    if(rv < 0)
      return AppError::opening_device(-rv,opts.device);

    captcha = captcha::calculate(blkdev);
    if(opts.captcha != captcha)
      return AppError::captcha(opts.captcha,captcha);

    if(input_file.empty())
      input_file = BadBlockFile::filepath(blkdev);

    rv = BadBlockFile::read(opts.input_file,badblocks);
    if(rv < 0)
      return AppError::reading_badblocks_file(-rv,opts.input_file);

    write_uncorrectable_loop(blkdev,badblocks);

    rv = blkdev.close();
    if(rv < 0)
      return AppError::closing_device(-rv,opts.device);

    return AppError::success();
  }
}
