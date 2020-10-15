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

#include <cmath>
#include <iomanip>
#include <sstream>

#include <stdint.h>


static const char *
suffix[][2] = {{ "B",   "B"},
               {"kB", "KiB"},
               {"MB", "MiB"},
               {"GB", "GiB"},
               {"TB", "TiB"},
               {"EB", "EiB"},
               {"ZB", "ZiB"},
               {"YB", "YiB"}};
static const int max_suffix = ((sizeof(suffix) / sizeof(suffix[0])) - 1);


static
int
compute_exp(const uint64_t bytes,
            const int      unit)
{
  double bytes_log = std::log((double)bytes);
  double unit_log  = std::log((double)unit);

  return std::min((int)(bytes_log / unit_log),max_suffix);
}

namespace num
{
  std::string
  humanize(const uint64_t bytes,
           const bool     si)
  {
    int exp  = 0;
    int unit = (si ? 1000 : 1024);
    std::stringstream ss;

    exp = ((bytes == 0) ?
           0 :
           compute_exp(bytes,unit));

    ss.precision(2);
    ss << std::fixed
       << (bytes / std::pow(unit,exp))
       << suffix[exp][si];

    return ss.str();
  }
}
