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

namespace l
{
  const
  char*
  form_factor(const sg::identity &ident_)
  {
    switch(ident_.form_factor)
      {
      default:
      case sg::FORM_FACTOR_UNKNOWN:
        return "unknown";
      case sg::FORM_FACTOR_5_25:
        return "5.25\"";
      case sg::FORM_FACTOR_3_5:
        return "3.5\"";
      case sg::FORM_FACTOR_2_5:
        return "2.5\"";
      case sg::FORM_FACTOR_1_8:
        return "1.8\"";
      case sg::FORM_FACTOR_LT_1_8:
        return "<1.8\"";
      }
  }
}

namespace bbf
{
  AppError
  info(const Options &opts)
  {
    int rv;
    BlkDev blkdev;
    sg::identity ident;

    rv = blkdev.open_read(opts.device);
    if(rv < 0)
      return AppError::opening_device(-rv,opts.device);

    std::cout << opts.device << ":" << std::endl;

    if(blkdev.has_identity())
      {
        ident = blkdev.identity();
        std::cout
          << " - serial_number: "             << ident.serial_number << std::endl
          << " - firmware_revision: "         << ident.firmware_revision << std::endl
          << " - model_number: "              << ident.model_number << std::endl
          << " - RPM: "                       << ident.rpm << std::endl
          << " - features:"                   << std::endl
          << "   - form_factor: "             << l::form_factor(ident) << std::endl
          << "   - write_uncorrectable: "     << ident.write_uncorrectable << std::endl
          << "   - smart_supported: "         << ident.smart_supported << std::endl
          << "   - smart_enabled: "           << ident.smart_enabled << std::endl
          << "   - security_supported: "      << ident.security_supported << std::endl
          << "   - security_enabled: "        << ident.security_enabled << std::endl
          << "   - security_locked: "         << ident.security_locked << std::endl
          << "   - security_frozen: "         << ident.security_frozen << std::endl
          << "   - security_count_expired: "  << ident.security_count_expired << std::endl
          << "   - security_enhanced_erase_supported: " << ident.security_enhanced_erase_supported << std::endl
          << "   - security_normal_erase_time: "   << ident.security_normal_erase_time << std::endl
          << "   - security_enhanced_erase_time: " << ident.security_enhanced_erase_time << std::endl
          << "   - block_erase: "             << ident.block_erase << std::endl
          << "   - overwrite: "               << ident.overwrite << std::endl
          << "   - crypto_scramble: "         << ident.crypto_scramble << std::endl
          << "   - sanitize: "                << ident.sanitize << std::endl
          << "   - supports_sata_gen1: "       << ident.supports_sata_gen1 << std::endl
          << "   - supports_sata_gen2: "       << ident.supports_sata_gen2 << std::endl
          << "   - supports_sata_gen3: "       << ident.supports_sata_gen3 << std::endl
          << "   - trim_supported: "          << ident.trim_supported << std::endl
          ;
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
