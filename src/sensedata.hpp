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

#ifndef __SENSEDATA_HPP__
#define __SENSEDATA_HPP__

#include <stdint.h>

namespace SenseData
{
  namespace SenseKey
  {
    enum SenseKey
      {
        NO_SENSE        = 0x00,
        RECOVERED_ERROR = 0x01,
        NOT_READY       = 0x02,
        MEDIUM_ERROR    = 0x03,
        HARDWARE_ERROR  = 0x04,
        ILLEGAL_REQUEST = 0x05,
        UNIT_ATTENTION  = 0x06,
        DATA_PROTECT    = 0x07,
        BLANK_CHECK     = 0x08,
        VENDOR_SPECIFIC = 0x09,
        COPY_ABORTED    = 0x0A,
        ABORTED_COMMAND = 0x0B,
        OBSOLETE        = 0x0C,
        VOLUME_OVERFLOW = 0x0D,
        MISCOMPARE      = 0x0E,
        COMPLETED       = 0x0F
      };
  }

  uint8_t response_code(const uint8_t * const sb);
  uint8_t sense_key(const uint8_t * const sb);
  uint8_t additional_sense_code(const uint8_t * const sb);
  uint8_t additional_sense_code_qualifier(const uint8_t * const sb);
  uint8_t additional_sense_length(const uint8_t * const sb);

  int asc_ascq_to_errno(const uint8_t asc,
                        const uint8_t ascq);
  int asc_ascq_to_errno(const uint8_t * const sb);

  const char * error_to_string(const int error);
}

#endif
