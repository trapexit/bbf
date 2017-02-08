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
#include <utility>

#include "badblockfile.hpp"
#include "blkdev.hpp"
#include "captcha.hpp"
#include "errors.hpp"
#include "info.hpp"
#include "math.hpp"
#include "options.hpp"
#include "signals.hpp"
#include "time.hpp"

static
int
write_read_compare(BlkDev         &blkdev,
                   const uint64_t  block,
                   const uint64_t  buffer_size,
                   const int       retries,
                   const char     *write_buf)
{
  int rv;
  char read_buf[buffer_size];

  rv = -1;
  for(uint64_t i = 0; ((i <= retries) && (rv < 0)); i++)
    rv = blkdev.write(block,write_buf,buffer_size);

  if(rv < 0)
    return rv;

  rv = -1;
  for(uint64_t i = 0; ((i <= retries) && (rv < 0)); i++)
    rv = blkdev.read(block,read_buf,buffer_size);

  if(rv < 0)
    return rv;

  rv = ::memcmp(write_buf,read_buf,buffer_size);
  if(rv != 0)
    return -EIO;

  return 0;
}

static
int
burn_block(BlkDev                         &blkdev,
           const uint64_t                  block,
           const uint64_t                  buffer_size,
           const uint64_t                  retries,
           const std::vector<const char*> &patterns)
{
  int rv;
  char orig_buf[buffer_size];

  rv = -1;
  for(uint64_t i = 0; ((i <= retries) && (rv < 0)); i++)
    rv = blkdev.read(block,orig_buf,buffer_size);

  if(rv < 0)
    ::memset(orig_buf,0,buffer_size);

  for(uint64_t i = 0, ei = patterns.size(); i != ei; i++)
    rv = write_read_compare(blkdev,block,buffer_size,retries,patterns[i]);

  return write_read_compare(blkdev,block,buffer_size,retries,orig_buf);
}

static
int
burnin_loop(BlkDev                &blkdev,
            const uint64_t         start_block,
            const uint64_t         end_block,
            const uint64_t         stepping,
            const uint64_t         buffer_size,
            std::vector<uint64_t> &badblocks,
            const int              retries)
{
  int rv;
  uint64_t block;
  double current_time;
  std::vector<const char*> patterns;
  const std::vector<char> buf_0x00(buffer_size,0x00);
  const std::vector<char> buf_0x55(buffer_size,0x55);
  const std::vector<char> buf_0xAA(buffer_size,0xAA);
  const std::vector<char> buf_0xFF(buffer_size,0xFF);
  const double start_time = Time::get_monotonic();

  patterns.push_back(&buf_0x00[0]);
  patterns.push_back(&buf_0x55[0]);
  patterns.push_back(&buf_0xAA[0]);
  patterns.push_back(&buf_0xFF[0]);

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

      rv = burn_block(blkdev,block,buffer_size,retries,patterns);
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
burnin(BlkDev                &blkdev,
       const Options         &opts,
       std::vector<uint64_t> &badblocks)
{
  int      retries;
  uint64_t buffer_size;
  uint64_t start_block;
  uint64_t end_block;
  uint64_t stepping;

  retries     = opts.retries;
  buffer_size = blkdev.physical_block_size();
  start_block = math::round_down(opts.start_block,blkdev.block_stepping());
  end_block   = std::min(opts.end_block,blkdev.logical_block_count());
  end_block   = math::round_up(end_block,blkdev.block_stepping());
  stepping    = blkdev.block_stepping();

  signals::alarm(1);

  std::cout << "\rBurning: " << start_block << " - " << end_block << std::endl;
  burnin_loop(blkdev,start_block,end_block,stepping,buffer_size,badblocks,retries);
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
burnin(const Options &opts)
{
  int      rv;
  int      retries;
  uint64_t start_block;
  uint64_t end_block;
  uint64_t num_of_blocks;
  BlkDev   blkdev;
  std::vector<uint64_t> badblocks;

  if(!opts.input_file.empty())
    {
      rv = BadBlockFile::read(opts.input_file,badblocks);
      if(rv < 0)
        return AppError::reading_badblocks_file(-rv,opts.input_file);
    }

  rv = blkdev.open_rdwr(opts.device);
  if(rv < 0)
    return AppError::opening_device(-rv,opts.device);

  const std::string captcha = captcha::calculate(blkdev);
  if(opts.captcha != captcha)
    return AppError::captcha(opts.captcha,captcha);

  set_blkdev_rwtype(blkdev,opts.rwtype);

  burnin(blkdev,opts,badblocks);

  rv = blkdev.close();

  rv = BadBlockFile::write(opts.output_file,badblocks);
  if(rv < 0)
    return AppError::writing_badblocks_file(-rv,opts.output_file);

  return AppError::success();
}

namespace bbf
{
  AppError
  burnin(const Options &opts)
  {
    return ::burnin(opts);
  }
}
