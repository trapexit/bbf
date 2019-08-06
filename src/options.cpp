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

#include "errors.hpp"
#include "options.hpp"

#include <string>
#include <iostream>

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BASE10 10

static
void
usage(std::ostream &os)
{
  os <<
    "usage: bbf [options] <instruction> <path>\n"
    "\n"
    "  instruction\n"
    "    * info                : print out details of the device\n"
    "    * captcha             : print captcha for device\n"
    "    * scan                : perform scan for bad blocks by reading\n"
    "    * fix                 : attempt to force drive to reallocate block\n"
    "                            - on successful read of block, write it back\n"
    "                            - on unsuccessful read of block, write zeros\n"
    "    * fix-file            : same behavior as 'fix' but specifically to a file's\n"
    "                            blocks\n"
    "    * burnin              : attempts a non-destructive write, read, & verify\n"
    "                            - read block, write block of 0x00, 0x55, 0xAA, 0xFF\n"
    "                            - write back original block if was successfully read\n"
    "                            - only if the last write,read,verify fails is it bad\n"
    "    * find-files          : given a list of bad blocks try to find affected files\n"
    "    * dump-files          : dump list of block ranges and files assocated with them\n"
    "    * file-blocks         : dump a list of individual blocks a file uses\n"
    "    * write-pseudo-uncorrectable-wl\n"
    "    * write-pseudo-uncorrectable-wol\n"
    "    * write-flagged-uncorrectable-wl\n"
    "    * write-flagged-uncorrectable-wol\n"
    "                          : mark blocks as pseudo or flagged uncorrectable\n"
    "                            with or without logging\n"
    "    * security-erase\n"
    "    * enhanced-security-erase\n"
    "                          : Erases all content on drive. Generally normal\n"
    "                            erase overwrites all users data with zeros and\n"
    "                            enhanced overwrites all data (including relocated)\n"
    "                            with vendor specific patterns.\n"
    "  path                    : block device|directory|file to act on\n"
    "\n"
    "  -f, --force             : normally destructive behavior fail if the device\n"
    "                            is mounted. This overrides this check.\n"
    "  -t, --rwtype <os|ata>   : use OS or ATA reads and writes (default: os)\n"
    "  -q, --quiet             : redirects stdout to /dev/null\n"
    "  -s, --start-block <lba> : block to start from (default: 0)\n"
    "  -e, --end-block <lba>   : block to stop at (default: last block)\n"
    "  -S, --stepping <n>      : number of logical blocks to read at a time\n"
    "                            (default: physical / logical)\n"
    "  -o, --output <file>     : file to write bad block list to\n"
    "                            defaults to ${HOME}/badblocks.<captcha>\n"
    "  -i, --input <file>      : file to read bad block list from\n"
    "                            defaults to ${HOME}/badblocks.<captcha>\n"
    "  -r, --retries <count>   : number of retries on certain reads & writes\n"
    "  -c, --captcha <captcha> : needed when performing destructive operations\n"
    "  -M, --max-errors <n>    : max r/w errors before exiting (default: 1024)\n"
    "\n";
}

AppError
Options::process_arg(const int          argc,
                     const char * const argv[],
                     const int          opt)
{
  switch(opt)
    {
    case 'f':
      force = true;
      break;
    case 'q':
      quiet++;
      break;
    case 'r':
      errno = 0;
      retries = ::strtol(optarg,NULL,BASE10);
      if(((errno == ERANGE) && (retries == LONG_MAX)) || (retries < 1))
        return AppError::argument_invalid("retries invalid");
      break;
    case 's':
      errno = 0;
      start_block = ::strtoull(optarg,NULL,BASE10);
      if((start_block == ULLONG_MAX) && (errno == ERANGE))
        return AppError::argument_invalid("start block value is invalid");
      break;
    case 'e':
      errno = 0;
      end_block = ::strtoull(optarg,NULL,BASE10);
      if((end_block == ULLONG_MAX) && (errno == ERANGE))
        return AppError::argument_invalid("end block value is invalid");
      break;
    case 'S':
      errno = 0;
      stepping = ::strtoull(optarg,NULL,BASE10);
      if((stepping == ULLONG_MAX) && (errno == ERANGE))
        return AppError::argument_invalid("invalid stepping");
      if((stepping > 65536) || (stepping <= 1))
        return AppError::argument_invalid("stepping must be >= 1 && <= 65536");
      break;
    case 'M':
      errno = 0;
      max_errors = ::strtoull(optarg,NULL,BASE10);
      if((max_errors == ULLONG_MAX) && (errno == ERANGE))
        return AppError::argument_invalid("max errors value is invalid");
      break;
    case 'o':
      output_file = optarg;
      break;
    case 'i':
      input_file = optarg;
      break;
    case 'c':
      captcha = optarg;
      break;
    case 't':
      if(!strcmp(optarg,"os"))
        rwtype = OS;
      else if(!strcmp(optarg,"ata"))
        rwtype = ATA;
      else
        return AppError::argument_invalid("valid rwtype values are 'os' or 'ata'");
      break;
    case 'h':
      usage(std::cout);
      return AppError::success();
    case '?':
      return AppError::argument_invalid(std::string(argv[optind-1]) + " is unknown");
    }

  return AppError::success();
}

Options::Instruction
Options::instr_from_string(const std::string str)
{
  if(str == "info")
    return Options::INFO;
  if(str == "captcha")
    return Options::CAPTCHA;
  if(str == "scan")
    return Options::SCAN;
  if(str == "fix")
    return Options::FIX;
  if(str == "fix-file")
    return Options::FIX_FILE;
  if(str == "burnin")
    return Options::BURNIN;
  if(str == "find-files")
    return Options::FIND_FILES;
  if(str == "dump-files")
    return Options::DUMP_FILES;
  if(str == "file-blocks")
    return Options::FILE_BLOCKS;
  if(str == "write-pseudo-uncorrectable-wl")
    return Options::WRITE_PSEUDO_UNCORRECTABLE_WL;
  if(str == "write-pseudo-uncorrectable-wol")
    return Options::WRITE_PSEUDO_UNCORRECTABLE_WOL;
  if(str == "write-flagged-uncorrectable-wl")
    return Options::WRITE_FLAGGED_UNCORRECTABLE_WL;
  if(str == "write-flagged-uncorrectable-wol")
    return Options::WRITE_FLAGGED_UNCORRECTABLE_WOL;
  if(str == "security-erase")
    return Options::SECURITY_ERASE;
  if(str == "enhanced-security-erase")
    return Options::ENHANCED_SECURITY_ERASE;

  return Options::_INVALID;
}

AppError
Options::parse(const int argc,
               char * const argv[])
{
  static const char short_options[] = "hqft:r:s:S:e:o:i:c:";
  static const struct option long_options[] =
    {
      {"help",              no_argument, NULL, 'h'},
      {"quiet",             no_argument, NULL, 'q'},
      {"force",             no_argument, NULL, 'f'},
      {"rwtype",      required_argument, NULL, 't'},
      {"retries",     required_argument, NULL, 'r'},
      {"start-block", required_argument, NULL, 's'},
      {"end-block",   required_argument, NULL, 'e'},
      {"stepping",    required_argument, NULL, 'S'},
      {"output",      required_argument, NULL, 'o'},
      {"input",       required_argument, NULL, 'i'},
      {"captcha",     required_argument, NULL, 'c'},
      {"max-errors",  required_argument, NULL, 'M'},
      {NULL,                          0, NULL,   0}
    };

  if(argc == 1)
    {
      usage(std::cout);
      return AppError::success();
    }

  opterr = 0;
  while(true)
    {
      int rv;
      AppError error;

      rv = getopt_long(argc,argv,short_options,long_options,NULL);
      if(rv == -1)
        break;

      error = process_arg(argc,argv,rv);
      if(!error.succeeded())
        return error;
    }

  if(argc == optind)
    return AppError::argument_required("instruction");
  if(argc == (optind + 1))
    return AppError::argument_required("target");

  instruction = instr_from_string(argv[optind]);
  device      = argv[optind+1];

  return validate();
}

AppError
Options::validate(void) const
{
  switch(instruction)
    {
    case Options::_INVALID:
      return AppError::argument_invalid("instruction is invalid");
    case Options::BURNIN:
      if(captcha.empty())
        return AppError::argument_required("captcha");
    case Options::SCAN:
      break;
    case Options::FIX:
    case Options::FIX_FILE:
    case Options::SECURITY_ERASE:
    case Options::ENHANCED_SECURITY_ERASE:
    case Options::WRITE_PSEUDO_UNCORRECTABLE_WL:
    case Options::WRITE_PSEUDO_UNCORRECTABLE_WOL:
    case Options::WRITE_FLAGGED_UNCORRECTABLE_WL:
    case Options::WRITE_FLAGGED_UNCORRECTABLE_WOL:
      if(captcha.empty())
        return AppError::argument_required("captcha");
    case Options::FIND_FILES:
      if(input_file.empty())
        return AppError::argument_required("input file");
      break;
    case Options::DUMP_FILES:
    case Options::INFO:
    case Options::CAPTCHA:
      break;
    default:
      break;
    }

  if(start_block >= end_block)
    return AppError::argument_invalid("start block >= end block");

  return AppError::success();
}
