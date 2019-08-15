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
#include "blocktofilemapper.hpp"
#include "errors.hpp"
#include "options.hpp"

#include <iostream>
#include <utility>

#include <stdint.h>

namespace bbf
{
  AppError
  find_files(const Options &opts)
  {
    int rv;
    BlockToFileMapper b2fm;
    std::vector<uint64_t> badblocks;

    rv = BadBlockFile::read(opts.input_file,badblocks);
    if(rv < 0)
      return AppError::reading_badblocks_file(-rv,opts.input_file);

    b2fm.scan(opts.device);

    const std::string none = "[none]";
    for(uint64_t i = 0, ei = badblocks.size(); i != ei; i++)
      {
        std::pair<bool,std::string> entry;
        const uint64_t badblock = badblocks[i];

        entry = b2fm.find(badblock);

        std::cout << badblock
                  << " "
                  << (entry.first ? entry.second : none)
                  << std::endl;
      }

    return AppError::success();
  }
}
