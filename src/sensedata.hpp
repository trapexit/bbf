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

namespace SenseData
{
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
