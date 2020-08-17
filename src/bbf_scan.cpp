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
#include "blkdev.hpp"
#include "errors.hpp"
#include "info.hpp"
#include "math.hpp"
#include "options.hpp"
#include "signals.hpp"
#include "time.hpp"

#include <iostream>
#include <iomanip>
#include <utility>

#include <errno.h>
#include <stdint.h>


namespace l
{
  static
  uint64_t
  trim_stepping(const BlkDev   &blkdev_,
                const uint64_t  block_,
                const uint64_t  stepping_)
  {
    uint64_t block_count;

    block_count = blkdev_.logical_block_count();

    if(block_ >= block_count)
      return 0;
    if((block_count - block_) < stepping_)
      return blkdev_.block_stepping();

    return stepping_;
  }

  static
  int
  scan_loop(BlkDev                &blkdev_,
            const uint64_t         stepping_,
            const uint64_t         start_block_,
            const uint64_t         end_block_,
            char                  *buf_,
            const uint64_t         buflen_,
            std::vector<uint64_t> &badblocks_,
            const uint64_t         max_errors_)
  {
    int rv;
    uint64_t block;
    uint64_t stepping;
    InfoPrinter info;
    const double start_time = Time::get_monotonic();

    info.init(start_block_,end_block_,&badblocks_);
    info.print(start_block_);

    block = start_block_;
    while(block < end_block_)
      {
        if(signals::signaled_to_exit())
          break;

        if(signals::dec(SIGALRM))
          {
            signals::alarm(1);
            info.print(block);
          }

        stepping = l::trim_stepping(blkdev_,block,stepping_);

        rv = blkdev_.read(block,stepping,buf_,buflen_);
        if(rv > 0)
          {
            block += stepping;
            continue;
          }

        if(rv == 0)
          break;
        if(rv > -256)
          break;

        info.print(block);

        for(uint64_t i = 0; i < stepping; i++)
          {
            rv = blkdev_.read(block+i,1,buf_,buflen_);
            if(rv > 0)
              continue;
            badblocks_.push_back(block+i);
          }

        if(badblocks_.size() > max_errors_)
          break;
      }

    info.print(block);

    return rv;
  }

  static
  AppError
  scan(BlkDev                &blkdev_,
       const Options         &opts_,
       std::vector<uint64_t> &badblocks_)
  {
    int rv;
    char *buf;
    uint64_t buflen;
    uint64_t start_block;
    uint64_t end_block;
    uint64_t stepping;

    stepping    = ((opts_.stepping == 0) ?
                   blkdev_.block_stepping() :
                   opts_.stepping);
    buflen      = (stepping * blkdev_.logical_block_size());
    start_block = math::round_down(opts_.start_block,stepping);
    end_block   = std::min(opts_.end_block,blkdev_.logical_block_count());
    end_block   = math::round_up(end_block,stepping);
    end_block   = std::min(end_block,blkdev_.logical_block_count());

    std::cout << "start block: "
              << start_block << std::endl
              << "end block: "
              << end_block << std::endl
              << "stepping: "
              << stepping << std::endl
              << "logical block size: "
              << blkdev_.logical_block_size() << std::endl
              << "physical block size: "
              << blkdev_.physical_block_size() << std::endl
              << "read size: "
              << stepping << " blocks / "
              << stepping * blkdev_.logical_block_size() << " bytes"
              << std::endl;

    signals::alarm(1);

    std::cout << "\r\x1B[2KScanning: "
              << start_block
              << " - "
              << end_block
              << std::endl;

    buf = new char[buflen];
    rv = l::scan_loop(blkdev_,
                      stepping,
                      start_block,
                      end_block,
                      buf,
                      buflen,
                      badblocks_,
                      opts_.max_errors);
    delete[] buf;

    std::cout << std::endl;

    if(rv < 0)
      return AppError::runtime(-rv,"error when scanning drive");

    return AppError::success();
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
  scan(const Options &opts_)
  {
    int rv;
    AppError err;
    BlkDev blkdev;
    std::string input_file;
    std::string output_file;
    std::vector<uint64_t> badblocks;

    input_file  = opts_.input_file;
    output_file = opts_.output_file;

    rv = blkdev.open_read(opts_.device);
    if(rv < 0)
      return AppError::opening_device(-rv,opts_.device);
    l::set_blkdev_rwtype(blkdev,opts_.rwtype);

    if(output_file.empty())
      output_file = BadBlockFile::filepath(blkdev);
    if(input_file.empty())
      input_file = output_file;

    rv = BadBlockFile::read(input_file,badblocks);
    if(rv > 0)
      std::cout << "Imported bad blocks from: " << input_file << std::endl;

    err = l::scan(blkdev,opts_,badblocks);

    rv = BadBlockFile::write(output_file,badblocks);
    if((rv < 0) && err.succeeded())
      err = AppError::writing_badblocks_file(-rv,output_file);
    else if(!badblocks.empty())
      std::cout << "Bad blocks written to: " << output_file << std::endl;

    rv = blkdev.close();
    if((rv < 0) && err.succeeded())
      err = AppError::closing_device(-rv,opts_.device);

    return err;
  }
}

namespace bbf
{
  AppError
  scan(const Options &opts_)
  {
    return l::scan(opts_);
  }
}
