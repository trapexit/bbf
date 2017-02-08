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

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

namespace signals
{
  volatile int _sigs[_NSIG] = {0};

  int
  get(const int sig)
  {
    return _sigs[sig];
  }

  void
  incr(const int sig)
  {
    _sigs[sig]++;
  }

  int
  dec(const int sig)
  {
    if(_sigs[sig] == 0)
      return 0;
    return _sigs[sig]--;
  }

  int
  signaled_to_exit(void)
  {
    if(signals::get(SIGTERM))
      return SIGTERM;
    if(signals::get(SIGINT))
      return SIGINT;
    if(signals::get(SIGQUIT))
      return SIGQUIT;

    return 0;
  }

  void
  handler(const int sig)
  {
    signals::incr(sig);
  }

  void
  setup_handlers()
  {
    struct sigaction sa = {0};

    sa.sa_flags   = SA_RESTART;
    sa.sa_handler = signals::handler;

    sigaction(SIGHUP,&sa,NULL);
    sigaction(SIGALRM,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGQUIT,&sa,NULL);
    sigaction(SIGUSR1,&sa,NULL);
    sigaction(SIGUSR2,&sa,NULL);
  }

  unsigned int
  alarm(const unsigned int secs)
  {
    return ::alarm(secs);
  }
}
