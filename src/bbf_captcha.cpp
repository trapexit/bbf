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

#include <iostream>

#include "blkdev.hpp"
#include "captcha.hpp"
#include "errors.hpp"
#include "filetoblkdev.hpp"
#include "options.hpp"

static
int
open_file_dev(BlkDev            &blkdev,
              const std::string &filepath)
{
  std::string devpath;

  devpath = FileToBlkDev::find(filepath);
  if(devpath.empty())
    return -ENOENT;

  return blkdev.open_read(devpath);
}

namespace bbf
{
  AppError
  captcha(const Options &opts)
  {
    int rv;
    BlkDev blkdev;

    rv = blkdev.open_read(opts.device);
    if(rv == -ENOTBLK)
      rv = open_file_dev(blkdev,opts.device);

    if(rv < 0)
      return AppError::opening_device(-rv,opts.device);

    std::cout << captcha::calculate(blkdev) << std::endl;

    rv = blkdev.close();
    if(rv < 0)
      return AppError::closing_device(-rv,opts.device);

    return AppError::success();
  }
}
