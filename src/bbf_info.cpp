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

#include <iostream>
#include <utility>

#include "blkdev.hpp"
#include "errors.hpp"
#include "num.hpp"
#include "options.hpp"

namespace bbf
{
  AppError
  info(const Options &opts)
  {
    int rv;
    BlkDev blkdev;

    rv = blkdev.open_read(opts.device);
    if(rv < 0)
      return AppError::opening_device(-rv,opts.device);

    std::cout << opts.device << ":" << std::endl;

    if(blkdev.has_identity())
      {
        std::cout
          << " - serial_number: "             << blkdev.serial_number() << std::endl
          << " - firmware_revision: "         << blkdev.firmware_revision() << std::endl
          << " - model_number: "              << blkdev.model_number() << std::endl
          << " - RPM: "                       << blkdev.rpm() << std::endl
          << " - features:"                   << std::endl
          << "   - write_uncorrectable_ext: " << blkdev.write_uncorrectable_ext() << std::endl
          << "   - smart_supported: "         << blkdev.smart_supported() << std::endl
          << "   - smart_enabled: "           << blkdev.smart_enabled() << std::endl;
      }

    std::cout
      << " - block_size:"  << std::endl
      << "   - physical: " << blkdev.physical_block_size() << std::endl
      << "   - logical: "  << blkdev.logical_block_size() << std::endl
      << "   - stepping: " << blkdev.block_stepping() << std::endl
      << " - block_count:" << std::endl
      << "   - physical: " << blkdev.physical_block_count() << std::endl
      << "   - logical: "  << blkdev.logical_block_count() << std::endl
      << " - size:"        << std::endl
      << "   - bytes: "    << blkdev.size_in_bytes() << std::endl
      << "   - human:"     << std::endl
      << "     - base2: "  << num::humanize(blkdev.size_in_bytes()) << std::endl
      << "     - base10: " << num::humanize(blkdev.size_in_bytes(),true) << std::endl
      << std::endl;

    rv = blkdev.close();
    if(rv < 0)
      return AppError::closing_device(-rv,opts.device);

    return AppError::success();
  }
}
