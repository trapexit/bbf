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

#ifndef __ERRORS_HPP__
#define __ERRORS_HPP__

#include <string>

namespace Error
{
  const char * to_string(const int error);
}

class AppError
{
public:
  enum Type
    {
      SUCCESS                = 0x00,
      ARGUMENT_REQUIRED      = 0x01,
      ARGUMENT_INVALID       = 0x02,
      OPENING_DEVICE         = 0x03,
      CLOSING_DEVICE         = 0x04,
      IDENTIFYING_DEVICE     = 0x05,
      READING_BADBLOCKS_FILE = 0x06,
      WRITING_BADBLOCKS_FILE = 0x07,
      OPENING_FILE           = 0x08,
      CAPTCHA                = 0x09,
      RUNTIME                = 0x0A,
      _END
    };

public:
  AppError() :
    type(SUCCESS),
    error(0),
    extra()
  {
  }

  AppError(const Type        _type,
           const int         _error,
           const std::string _extra = std::string()) :
    type(_type),
    error(_error),
    extra(_extra)
  {
  }

public:
  static const AppError success();
  static const AppError argument_required(const std::string&);
  static const AppError argument_invalid(const std::string&);
  static const AppError not_a_directory(const std::string&);
  static const AppError opening_device(const int, const std::string&);
  static const AppError closing_device(const int, const std::string&);
  static const AppError identifying_device(const int, const std::string&);
  static const AppError reading_badblocks_file(const int, const std::string&);
  static const AppError writing_badblocks_file(const int, const std::string&);
  static const AppError opening_file(const int, const std::string&);
  static const AppError captcha(const std::string&,const std::string&);
  static const AppError runtime(const int,const std::string&);

public:
  bool succeeded(void) const;
  std::string to_string(void) const;

public:
  Type        type;
  int         error;
  std::string extra;
};

#endif
