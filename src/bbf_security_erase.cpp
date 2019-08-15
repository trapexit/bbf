/*
  ISC License

  Copyright (c) 2019, Antonio SJ Musumeci <trapexit@spawn.link>

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

#include "blkdev.hpp"
#include "captcha.hpp"
#include "errors.hpp"
#include "info.hpp"
#include "math.hpp"
#include "options.hpp"
#include "signals.hpp"
#include "time.hpp"
#include "sg.hpp"

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

#include <errno.h>
#include <stdint.h>

static const uint64_t TIMEOUT_15_SECS = (15 * 1000);

namespace l
{
  static
  std::string
  random_str(void)
  {
    std::string str;

    std::srand(std::time(NULL));

    for(uint32_t i = 0; i < 6; i++)
      str.push_back('a' + (std::rand() % 26));

    return str;
  }

  static
  uint64_t
  estimated_time(const BlkDev &blkdev_)
  {
    if(blkdev_.has_identity() == false)
      return (12 * 60 * 60 * 1000);

    /* hdparm uses a 1s / 30MB of capacity estimate, we'll use that too  */
    return (blkdev_.size_in_bytes() / (30 * 1024 * 1024) * 1000);
  }

  static
  uint64_t
  reported_time(const BlkDev &blkdev_,
                const bool    enhanced_)
  {
    if(blkdev_.has_identity() == false)
      return (12 * 60 * 60 * 1000);

    return ((enhanced_ ?
             blkdev_.identity().security_enhanced_erase_time :
             blkdev_.identity().security_normal_erase_time) * 60 * 1000);
  }

  static
  AppError
  security_erase(const BlkDev  &blkdev_,
                 const Options &opts_,
                 const bool     enhanced_)
  {
    int rv;
    uint64_t reported_time;
    uint64_t estimated_time;
    uint64_t erase_time;
    std::string input;
    std::string captcha;
    std::string password;

    if(blkdev_.identity().security_frozen == true)
      return AppError::runtime(EBUSY,"Security frozen. Unable to continue.");

    reported_time  = l::reported_time(blkdev_,enhanced_);
    estimated_time = l::estimated_time(blkdev_);
    erase_time     = std::max(reported_time,estimated_time);
    std::cout << "Security erase time: ~"
              << (reported_time / (60 * 1000))
              << " minutes (reported) & ~"
              << (estimated_time / (60 * 1000))
              << " minutes (estimated from 1s/30MB)"
              << std::endl;

    while(password.empty())
      {
        std::cout << "Enter "
                  << (blkdev_.identity().security_enabled ? "the" : "a")
                  << " drive password (will be reset after erase): ";

        std::cin.clear();
        std::getline(std::cin,password);
        if(password.empty())
          std::cout << "ERROR: reenter password - can not be empty!" << std::endl;
      }

    password.resize(32,'\0');

    captcha = random_str();
    std::cout << "Enter the following to confirm erase - '" << captcha << "': ";
    std::getline(std::cin,input);
    if(input != captcha)
      return AppError::captcha(input,captcha);

    if(blkdev_.identity().security_enabled == false)
      {
        rv = sg::security_set_password(blkdev_.fd(),
                                       sg::SG_IDENTIFIER_USER,
                                       password.c_str(),
                                       TIMEOUT_15_SECS);
        if(rv < 0)
          return AppError::runtime(-rv,"failed to set password");
        std::cout << "Security password set successfully" << std::endl;
      }

    rv = sg::security_erase_prepare(blkdev_.fd(),TIMEOUT_15_SECS);
    if(rv < 0)
      return AppError::runtime(-rv,"security erase prepare instruction failed");
     std::cout << "Security erase prepare command issued successfully" << std::endl;

    std::cout << "Security erase starting" << std::endl;

    rv = sg::security_erase(blkdev_.fd(),
                            enhanced_ ? sg::SG_ERASE_ENHANCED : sg::SG_ERASE_NORMAL,
                            sg::SG_IDENTIFIER_USER,
                            password.c_str(),
                            erase_time);
    if(rv < 0)
      return AppError::runtime(-rv,"security erase instruction failed");
    std::cout << "Security erase finished" << std::endl;

    return AppError::success();
  }

  static
  AppError
  security_erase(const Options &opts_,
                 const bool     enhanced_)
  {
    int rv;
    AppError err;
    BlkDev blkdev;

    rv = blkdev.open_read(opts_.device);
    if(rv < 0)
      return AppError::opening_device(-rv,opts_.device);

    const std::string captcha = captcha::calculate(blkdev);
    if(opts_.captcha != captcha)
      return AppError::captcha(opts_.captcha,captcha);

    err = l::security_erase(blkdev,opts_,enhanced_);

    rv = blkdev.close();
    if((rv < 0) && err.succeeded())
      err = AppError::closing_device(-rv,opts_.device);

    return err;
  }
}

namespace bbf
{
  AppError
  security_erase(const Options &opts_)
  {
    return l::security_erase(opts_,false);
  }

  AppError
  enhanced_security_erase(const Options &opts_)
  {
    return l::security_erase(opts_,true);
  }
}
