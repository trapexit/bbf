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

#include "info.hpp"
#include "time.hpp"

#include <iostream>
#include <iomanip>
#include <vector>

#include <stdint.h>

using namespace std;

InfoPrinter::InfoPrinter()
  : _start_time(0),
    _start_block(0),
    _end_block(0),
    _badblocks(NULL)
{

}

void
InfoPrinter::init(const size_t  start_block_,
                  const size_t  end_block_,
                  U64Vec       *badblocks_)
{
  std::cout << "\r\x1B[2K" << '\x1B' << '7';

  _start_block = start_block_;
  _end_block   = end_block_;
  _badblocks   = badblocks_;
  _start_time  = Time::get_monotonic();
}

void
InfoPrinter::print(const size_t current_block_)
{
  const double current_time      = Time::get_monotonic();
  const size_t badcount          = _badblocks->size();
  const size_t processed_blocks  = (current_block_ - _start_block);
  const size_t total_blocks      = (_end_block - _start_block);
  const size_t blocks_left       = (total_blocks - processed_blocks);
  const double percentage        = (((double)processed_blocks / total_blocks) * 100.0);
  const double time_passed       = (current_time - _start_time);
  const double blocks_per_second = ((double)processed_blocks / time_passed);
  const size_t time_left         = ((double)blocks_left / blocks_per_second);

  std::cout << "\x1B" << '8'
            << "\x1B[2K"
            << "Current: " << current_block_ << " ("
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
    std::cout << "; last: " << (*_badblocks)[badcount-1];
  std::cout << std::endl << "\x1B[2K" << std::flush;
}
