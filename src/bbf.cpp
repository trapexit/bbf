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

#include "bbf_burnin.hpp"
#include "bbf_captcha.hpp"
#include "bbf_dump_files.hpp"
#include "bbf_file_blocks.hpp"
#include "bbf_find_files.hpp"
#include "bbf_fix.hpp"
#include "bbf_fix_file.hpp"
#include "bbf_info.hpp"
#include "bbf_scan.hpp"
#include "bbf_security_erase.hpp"
#include "bbf_write_uncorrectable.hpp"

#include "errors.hpp"
#include "options.hpp"
#include "signals.hpp"

#include <iostream>
#include <utility>

namespace l
{
  static
  AppError
  process_instruction(const Options &opts_)
  {
    if(opts_.quiet)
      std::cout.setstate(std::ios::failbit);

    switch(opts_.instruction)
      {
      case Options::INFO:
        return bbf::info(opts_);
      case Options::SCAN:
        return bbf::scan(opts_);
      case Options::FIX:
        return bbf::fix(opts_);
      case Options::FIX_FILE:
        return bbf::fix_file(opts_);
      case Options::FIND_FILES:
        return bbf::find_files(opts_);
      case Options::FILE_BLOCKS:
        return bbf::file_blocks(opts_);
      case Options::DUMP_FILES:
        return bbf::dump_files(opts_);
      case Options::WRITE_PSEUDO_UNCORRECTABLE_WL:
      case Options::WRITE_PSEUDO_UNCORRECTABLE_WOL:
      case Options::WRITE_FLAGGED_UNCORRECTABLE_WL:
      case Options::WRITE_FLAGGED_UNCORRECTABLE_WOL:
        return bbf::write_uncorrectable(opts_);
      case Options::BURNIN:
        return bbf::burnin(opts_);
      case Options::CAPTCHA:
        return bbf::captcha(opts_);
      case Options::SECURITY_ERASE:
        return bbf::security_erase(opts_);
      case Options::ENHANCED_SECURITY_ERASE:
        return bbf::enhanced_security_erase(opts_);
      }

    return AppError::success();
  }
}

int
main(int    argc_,
     char **argv_)
{
  AppError rv;
  Options opts;

  signals::setup_handlers();

  rv = opts.parse(argc_,argv_);
  if(rv.succeeded())
    rv = l::process_instruction(opts);

  if(!rv.succeeded())
    std::cerr << rv.to_string() << std::endl;

  return rv.type;
}
