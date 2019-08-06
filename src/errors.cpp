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
#include <string.h>

#include "errors.hpp"
#include "sensedata.hpp"

namespace Error
{
  const
  char *
  to_string(const int error)
  {
    if(error < 256)
      return ::strerror(error);
    return SenseData::error_to_string(error);
  }
}

static
std::string
type_to_string(const int type)
{
  switch(type)
    {
    case AppError::SUCCESS:
      return "success";
    case AppError::ARGUMENT_REQUIRED:
      return "required argument";
    case AppError::ARGUMENT_INVALID:
      return "invalid argument";
    case AppError::OPENING_DEVICE:
      return "unable to open device";
    case AppError::CLOSING_DEVICE:
      return "unable to close device";
    case AppError::IDENTIFYING_DEVICE:
      return "unable to identify device";
    case AppError::READING_BADBLOCKS_FILE:
      return "unable to read bad blocks file";
    case AppError::WRITING_BADBLOCKS_FILE:
      return "unable to write bad blocks file";
    case AppError::OPENING_FILE:
      return "unable to open file";
    case AppError::CAPTCHA:
      return "captcha doesn't match";
    case AppError::RUNTIME:
      return "runtime";
    default:
      return ":unknown:";
    }
}

bool
AppError::succeeded(void) const
{
  return (type == SUCCESS);
}

std::string
AppError::to_string(void) const
{
  std::string rv;

  rv = "ERROR - ";
  rv += type_to_string(type);
  rv += ": ";
  rv += extra;
  rv += " [";
  rv += Error::to_string(error);
  rv += ']';

  return rv;
}

const
AppError
AppError::success(void)
{
  return AppError(SUCCESS,0);
}

const
AppError
AppError::argument_required(const std::string &_extra)
{
  return AppError(ARGUMENT_REQUIRED,EINVAL,_extra);
}

const
AppError
AppError::argument_invalid(const std::string &_extra)
{
  return AppError(ARGUMENT_INVALID,EINVAL,_extra);
}

const
AppError
AppError::opening_device(const int          _error,
                         const std::string &_extra)
{
  return AppError(OPENING_DEVICE,_error,_extra);
}

const
AppError
AppError::closing_device(const int          _error,
                         const std::string &_extra)
{
  return AppError(CLOSING_DEVICE,_error,_extra);
}

const
AppError
AppError::identifying_device(const int          _error,
                             const std::string &_extra)
{
  return AppError(IDENTIFYING_DEVICE,_error,_extra);
}

const
AppError
AppError::reading_badblocks_file(const int          _error,
                                 const std::string &_extra)
{
  return AppError(READING_BADBLOCKS_FILE,_error,_extra);
}

const
AppError
AppError::writing_badblocks_file(const int          _error,
                                 const std::string &_extra)
{
  return AppError(WRITING_BADBLOCKS_FILE,_error,_extra);
}

const
AppError
AppError::opening_file(const int          _error,
                       const std::string &_extra)
{
  return AppError(OPENING_FILE,_error,_extra);
}

const
AppError
AppError::captcha(const std::string &_captcha0,
                  const std::string &_captcha1)
{
  return AppError(CAPTCHA,EINVAL,_captcha0 + " != " + _captcha1);
}

const
AppError
AppError::runtime(const int          error_,
                  const std::string &extra_)
{
  return AppError(RUNTIME,error_,extra_);
}
