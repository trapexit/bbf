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
#include <stdint.h>

#include <iostream>
#include <iomanip>
#include <utility>

#include "badblockfile.hpp"
#include "blkdev.hpp"
#include "errors.hpp"
#include "info.hpp"
#include "math.hpp"
#include "options.hpp"
#include "signals.hpp"
#include "time.hpp"

static
int
is_badblock(BlkDev         &blkdev,
            const uint64_t  block,
            const uint64_t  physical_block_size)
{
  char buf[physical_block_size];

  return blkdev.read(block,buf,physical_block_size);
}

static
int
is_badblock(BlkDev         &blkdev,
            const uint64_t  block)
{
  return is_badblock(blkdev,block,blkdev.physical_block_size());
}

static
int
scan_loop(BlkDev                &blkdev,
          const uint64_t         start_block,
          const uint64_t         end_block,
          const uint64_t         stepping,
          std::vector<uint64_t> &badblocks)
{
  int rv;
  uint64_t block;
  double current_time;
  const double start_time = Time::get_monotonic();

  current_time = Time::get_monotonic();
  Info::print(std::cout,start_time,current_time,
              start_block,end_block,start_block,badblocks);

  for(block  = start_block;
      block != end_block;
      block += stepping)
    {
      if(signals::signaled_to_exit())
        break;

      if(signals::dec(SIGALRM))
        {
          signals::alarm(1);
          current_time = Time::get_monotonic();
          Info::print(std::cout,start_time,current_time,
                      start_block,end_block,block,badblocks);
        }

      rv = is_badblock(blkdev,block);
      if(rv >= 0)
        continue;

      current_time = Time::get_monotonic();
      Info::print(std::cout,start_time,current_time,
                  start_block,end_block,block,badblocks);
      for(uint64_t i = 0; i < stepping; i++)
        badblocks.push_back(block+i);
    }

  current_time = Time::get_monotonic();
  Info::print(std::cout,start_time,current_time,
              start_block,end_block,block,badblocks);

  return 0;
}

static
int
scan(BlkDev                &blkdev,
     const Options         &opts,
     std::vector<uint64_t> &badblocks)
{
  uint64_t start_block;
  uint64_t end_block;
  uint64_t stepping;

  start_block = math::round_down(opts.start_block,blkdev.block_stepping());
  end_block   = std::min(opts.end_block,blkdev.logical_block_count());
  end_block   = math::round_up(end_block,blkdev.block_stepping());
  stepping    = blkdev.block_stepping();

  signals::alarm(1);

  std::cout << "\r\x1B[2KScanning: " << start_block << " - " << end_block << std::endl;
  scan_loop(blkdev,start_block,end_block,stepping,badblocks);
  std::cout << std::endl;

  return 0;
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
scan(const Options &opts)
{
  int rv;
  BlkDev blkdev;
  std::string input_file;
  std::string output_file;
  std::vector<uint64_t> badblocks;

  input_file  = opts.input_file;
  output_file = opts.output_file;

  rv = blkdev.open_read(opts.device);
  if(rv < 0)
    return AppError::opening_device(-rv,opts.device);

  if(output_file.empty())
    output_file = BadBlockFile::filepath(blkdev);

  if(input_file.empty())
    input_file = output_file;

  rv = BadBlockFile::read(input_file,badblocks);
  if(rv < 0)
    std::cout << "Warning: unable to open " << input_file << std::endl;

  set_blkdev_rwtype(blkdev,opts.rwtype);

  scan(blkdev,opts,badblocks);

  rv = BadBlockFile::write(output_file,badblocks);
  if(rv < 0)
    return AppError::writing_badblocks_file(-rv,output_file);
  std::cout << "Bad blocks written to " << output_file << std::endl;

  rv = blkdev.close();
  if(rv < 0)
    return AppError::closing_device(-rv,opts.device);

  return AppError::success();
}

namespace bbf
{
  AppError
  scan(const Options &opts)
  {
    return ::scan(opts);
  }
}
