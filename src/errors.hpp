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
      SUCCESS                = 0,
      ARGUMENT_REQUIRED      = 1,
      ARGUMENT_INVALID       = 2,
      OPENING_DEVICE         = 3,
      CLOSING_DEVICE         = 4,
      IDENTIFYING_DEVICE     = 5,
      READING_BADBLOCKS_FILE = 6,
      WRITING_BADBLOCKS_FILE = 7,
      OPENING_FILE           = 8,
      CAPTCHA                = 9,
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
  static const AppError opening_device(const int, const std::string&);
  static const AppError closing_device(const int, const std::string&);
  static const AppError identifying_device(const int, const std::string&);
  static const AppError reading_badblocks_file(const int, const std::string&);
  static const AppError writing_badblocks_file(const int, const std::string&);
  static const AppError opening_file(const int, const std::string&);
  static const AppError captcha(const std::string&,const std::string&);

public:
  bool succeeded(void) const;
  std::string to_string(void) const;

public:
  Type        type;
  int         error;
  std::string extra;
};

#endif
