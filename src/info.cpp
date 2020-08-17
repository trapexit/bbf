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
#include <iomanip>
#include <vector>

#include <stdint.h>

using namespace std;

namespace Info
{
  void
  print(ostream                &os,
        const double            start_time,
        const double            current_time,
        const size_t            start_block,
        const size_t            end_block,
        const size_t            current_block,
        const vector<uint64_t> &badblocks)
  {
    const size_t badcount          = badblocks.size();
    const size_t processed_blocks  = (current_block - start_block);
    const size_t total_blocks      = (end_block - start_block);
    const size_t blocks_left       = (total_blocks - processed_blocks);
    const double percentage        = (((double)processed_blocks / total_blocks) * 100.0);
    const double time_passed       = (current_time - start_time);
    const double blocks_per_second = ((double)processed_blocks / time_passed);
    const size_t time_left         = ((double)blocks_left / blocks_per_second);

    os << "\r\x1B[2KCurrent: " << current_block << " ("
       << std::fixed
       << std::setprecision(2)
       << percentage
       << "%); bps: " << blocks_per_second
       << "; eta: "
       << std::setfill('0') << std::setw(2)
       << (time_left/(60*60)) << ':'
       << std::setfill('0') << std::setw(2)
       << ((time_left/60)%60)  << ':'
       << std::setfill('0') << std::setw(2)
       << (time_left%60)
       << "; bad: " << badcount;
    if(badcount)
      os << "; last: " << badblocks[badcount-1];
    os << std::flush;
  }
}
