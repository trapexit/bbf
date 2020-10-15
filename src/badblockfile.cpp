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
#include "captcha.hpp"
#include "blkdev.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdint.h>
#include <stdlib.h>


std::string
BadBlockFile::filepath(const BlkDev &blkdev_)
{
  const char *homedir;
  std::stringstream ss;

  homedir = ::getenv("HOME");
  if((homedir == NULL) || (homedir[0] == '\0'))
    homedir = "/root";

  ss << homedir << "/badblocks." << captcha::calculate(blkdev_);

  return ss.str();
}

namespace l
{
  static
  void
  read(std::istream          &stream_,
       std::vector<uint64_t> &blocks_)
  {
    while(!stream_.eof())
      {
        uint64_t block;

        stream_ >> block;

        if(!stream_.eof())
          blocks_.push_back(block);
      }
  }
}

int
BadBlockFile::read(const std::string     &filepath_,
                   std::vector<uint64_t> &blocks_)
{
  if(filepath_ == "-")
    {
      l::read(std::cin,blocks_);
    }
  else
    {
      std::ifstream file;

      file.open(filepath_.c_str());
      if(!file.is_open() || file.bad())
        return -1;

      l::read(file,blocks_);

      file.close();
    }

  return 0;
}

namespace l
{
  static
  void
  write(std::ostream                &stream_,
        const std::vector<uint64_t> &blocks_)
  {
    for(size_t i = 0, ei = blocks_.size(); i != ei; i++)
      stream_ << blocks_[i] << std::endl;
  }
}

int
BadBlockFile::write(const std::string           &filepath_,
                    const std::vector<uint64_t> &blocks_)
{
  if(filepath_ == "-")
    {
      l::write(std::cout,blocks_);
    }
  else
    {
      std::ofstream file;

      file.open(filepath_.c_str());
      if(!file.is_open() || file.bad())
        return -1;

      l::write(file,blocks_);

      file.close();
    }

  return 0;
}
