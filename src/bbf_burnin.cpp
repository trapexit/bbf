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
#include "captcha.hpp"
#include "errors.hpp"
#include "info.hpp"
#include "math.hpp"
#include "options.hpp"
#include "signals.hpp"
#include "time.hpp"

#include <iostream>
#include <utility>

#include <errno.h>
#include <stdint.h>

typedef std::vector<char> CharVec;
typedef std::vector<CharVec> CharVecVec;

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
  write_read_compare(BlkDev         &blkdev_,
                     const uint64_t  stepping_,
                     const uint64_t  block_,
                     CharVec        &tmpbuf_,
                     const int       retries_,
                     const CharVec  &write_buf_)
  {
    int rv;

    rv = -1;
    for(uint64_t i = 0; ((i <= retries_) && (rv < 0)); i++)
      rv = blkdev_.write(block_,stepping_,write_buf_);

    if(rv < 0)
      return rv;

    rv = -1;
    for(uint64_t i = 0; ((i <= retries_) && (rv < 0)); i++)
      rv = blkdev_.read(block_,stepping_,tmpbuf_);

    if(rv < 0)
      return rv;

    rv = ::memcmp(&write_buf_[0],&tmpbuf_[0],tmpbuf_.size());
    if(rv != 0)
      return -EIO;

    return 0;
  }

  static
  int
  burn_block(BlkDev            &blkdev_,
             const uint64_t     stepping_,
             const uint64_t     block_,
             CharVec           &tmpbuf_,
             CharVec           &savebuf_,
             const uint64_t     retries_,
             const CharVecVec  &patterns_)
  {
    int rv;

    rv = -1;
    for(uint64_t i = 0; ((i <= retries_) && (rv < 0)); i++)
      rv = blkdev_.read(block_,stepping_,savebuf_);

    if(rv < 0)
      ::memset(&savebuf_[0],0,savebuf_.size());

    for(uint64_t i = 0; i < patterns_.size(); i++)
      rv = l::write_read_compare(blkdev_,stepping_,block_,tmpbuf_,retries_,patterns_[i]);

    rv = -1;
    for(uint64_t i = 0; ((i <= retries_) && (rv < 0)); i++)
      rv = blkdev_.write(block_,stepping_,savebuf_);

    return rv;
  }

  static
  int
  burnin_loop(BlkDev                &blkdev_,
              const uint64_t         start_block_,
              const uint64_t         end_block_,
              const uint64_t         stepping_,
              const uint64_t         buflen_,
              std::vector<uint64_t> &badblocks_,
              const uint64_t         max_errors_,
              const int              retries_)
  {
    int rv;
    uint64_t block;
    uint64_t stepping;
    InfoPrinter info;
    CharVec tmpbuf;
    CharVec savebuf;
    CharVecVec patterns;
    const double start_time = Time::get_monotonic();

    tmpbuf.resize(buflen_,0x00);
    savebuf.resize(buflen_,0x00);

    patterns.resize(4);
    patterns[0].resize(buflen_,0x00);
    patterns[1].resize(buflen_,0x55);
    patterns[2].resize(buflen_,0xAA);
    patterns[3].resize(buflen_,0xFF);

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

        rv = l::burn_block(blkdev_,stepping,block,tmpbuf,savebuf,retries_,patterns);

        block += stepping;
        if(rv >= 0)
          continue;
        if(rv == -EINVAL)
          break;

        info.print(block);

        for(uint64_t i = 0; i < stepping; i++)
          badblocks_.push_back(block+i);

        if(badblocks_.size() > max_errors_)
          break;
      }

    info.print(block);

    return rv;
  }

  static
  AppError
  burnin(BlkDev                &blkdev_,
         const Options         &opts_,
         std::vector<uint64_t> &badblocks_)
  {
    int       rv;
    int       retries;
    size_t    buflen;
    uint64_t  start_block;
    uint64_t  end_block;
    uint64_t  stepping;

    retries     = opts_.retries;
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
              << "r/w size: "
              << stepping << " blocks / "
              << buflen << " bytes"
              << std::endl;

    signals::alarm(1);

    std::cout << "\r\x1B[2KBurning: "
              << start_block
              << " - "
              << end_block
              << std::endl;

    rv = l::burnin_loop(blkdev_,
                        start_block,
                        end_block,
                        stepping,
                        buflen,
                        badblocks_,
                        opts_.max_errors,
                        retries);

    std::cout << std::endl;

    if(rv < 0)
      return AppError::runtime(-rv,"error when performing burnin");

    return AppError::success();
  }

  static
  void
  set_blkdev_rwtype(BlkDev                &blkdev_,
                    const Options::RWType  rwtype_)
  {
    switch(rwtype_)
      {
      case Options::ATA:
        blkdev_.set_rw_ata();
        break;
      case Options::OS:
        blkdev_.set_rw_os();
        break;
      }
  }

  static
  AppError
  burnin(const Options &opts_)
  {
    int rv;
    AppError err;
    BlkDev blkdev;
    std::string captcha;
    std::string input_file;
    std::string output_file;
    std::vector<uint64_t> badblocks;

    input_file  = opts_.input_file;
    output_file = opts_.output_file;

    rv = blkdev.open_rdwr(opts_.device,!opts_.force);
    if(rv < 0)
      return AppError::opening_device(-rv,opts_.device);

    captcha = captcha::calculate(blkdev);
    if(opts_.captcha != captcha)
      return AppError::captcha(opts_.captcha,captcha);

    if(output_file.empty())
      output_file = BadBlockFile::filepath(blkdev);

    if(input_file.empty())
      input_file = output_file;

    rv = BadBlockFile::read(input_file,badblocks);
    if(rv < 0)
      std::cout << "Warning: unable to open " << input_file << std::endl;
    else
      std::cout << "Imported bad blocks from " << input_file << std::endl;

    l::set_blkdev_rwtype(blkdev,opts_.rwtype);

    err = l::burnin(blkdev,opts_,badblocks);

    rv = BadBlockFile::write(output_file,badblocks);
    if((rv < 0) && err.succeeded())
      err = AppError::writing_badblocks_file(-rv,output_file);
    else if(!badblocks.empty())
      std::cout << "Bad blocks written to " << output_file << std::endl;

    rv = blkdev.close();
    if((rv < 0) && err.succeeded())
      err = AppError::closing_device(-rv,opts_.device);

    return err;
  }
}

namespace bbf
{
  AppError
  burnin(const Options &opts_)
  {
    return l::burnin(opts_);
  }
}
