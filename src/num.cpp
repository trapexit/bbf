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
