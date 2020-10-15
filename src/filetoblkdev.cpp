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

#include <string>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


namespace FileToBlkDev
{
  static
  dev_t
  st_dev(const std::string &filepath_)
  {
    int rv;
    struct stat st;

    rv = ::lstat(filepath_.c_str(),&st);

    return ((rv == -1) ? -1 : st.st_dev);
  }

  static
  dev_t
  st_rdev(const std::string &filepath_)
  {
    int rv;
    struct stat st;

    rv = ::lstat(filepath_.c_str(),&st);

    return ((rv == -1) ? -1 : st.st_rdev);
  }

  static
  std::string
  find(const dev_t device_)
  {
    DIR *dir;
    std::string devpath;
    const std::string devstr("/dev/");

    dir = ::opendir("/dev");
    if(dir == NULL)
      return std::string();

    for(struct dirent *d = ::readdir(dir); d != NULL; d = ::readdir(dir))
      {
        dev_t rdev;

        devpath = devstr + d->d_name;

        rdev = FileToBlkDev::st_rdev(devpath);
        if(rdev == device_)
          {
            ::closedir(dir);
            return devpath;
          }
      }

    ::closedir(dir);

    return std::string();
  }

  std::string
  find(const std::string &filepath_)
  {
    dev_t device;

    device = FileToBlkDev::st_dev(filepath_);

    return FileToBlkDev::find(device);
  }
}
