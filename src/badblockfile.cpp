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

#include <stdint.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "badblockfile.hpp"
#include "captcha.hpp"
#include "blkdev.hpp"

std::string
BadBlockFile::filepath(const BlkDev &blkdev)
{
  const char *homedir;
  std::stringstream ss;

  homedir = ::getenv("HOME");
  if((homedir == NULL) || (homedir[0] == '\0'))
    homedir = "/root";

  ss << homedir << "/badblocks." << captcha::calculate(blkdev);

  return ss.str();
}

static
void
_read(std::istream          &stream,
      std::vector<uint64_t> &blocks)
{
  while(!stream.eof())
    {
      uint64_t block;

      stream >> block;

      if(!stream.eof())
        blocks.push_back(block);
    }
}

int
BadBlockFile::read(const std::string     &filepath,
                   std::vector<uint64_t> &blocks)
{
  if(filepath == "-")
    {
      _read(std::cin,blocks);
    }
  else
    {
      std::ifstream file;

      file.open(filepath.c_str());
      if(!file.is_open() || file.bad())
        return -1;

      _read(file,blocks);

      file.close();
    }

  return 0;
}

static
void
_write(std::ostream                &stream,
       const std::vector<uint64_t> &blocks)
{
  for(size_t i = 0, ei = blocks.size(); i != ei; i++)
    stream << blocks[i] << std::endl;
}

int
BadBlockFile::write(const std::string           &filepath,
                    const std::vector<uint64_t> &blocks)
{
  if(filepath == "-")
    {
      _write(std::cout,blocks);
    }
  else
    {
      std::ofstream file;

      file.open(filepath.c_str());
      if(!file.is_open() || file.bad())
        return -1;

      _write(file,blocks);

      file.close();
    }

  return 0;
}
