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

#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__

#include <stdint.h>

#include <string>

struct Options
{
public:
  enum Instruction
    {
      _INVALID = -1,
      BURNIN,
      CAPTCHA,
      DUMP_FILES,
      FILE_BLOCKS,
      FIND_FILES,
      FIX,
      INFO,
      SCAN,
      WRITE_UNCORRECTABLE,
      _END
    };

  enum RWType
    {
      ATA,
      OS
    };

public:
  Options() :
    quiet(0),
    retries(0),
    start_block(0),
    end_block(~0ULL),
    output_file(),
    input_file(),
    instruction(_INVALID),
    device(),
    rwtype(OS),
    force(false)
  {}

public:
  AppError parse(const int argc, char * const argv[]);

private:
  AppError    process_arg(const int          argc,
                          const char * const argv[],
                          const int          opt);
  AppError    validate(void) const;
  Instruction instr_from_string(const std::string str);

public:
  Instruction instruction;
  RWType      rwtype;
  std::string device;

  int         quiet;
  long        retries;
  uint64_t    start_block;
  uint64_t    end_block;
  std::string output_file;
  std::string input_file;
  std::string captcha;
  bool        force;
};

#endif
