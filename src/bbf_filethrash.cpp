/*
  ISC License

  Copyright (c) 2020, Antonio SJ Musumeci <trapexit@spawn.link>

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

#include "info.hpp"
#include "captcha.hpp"
#include "options.hpp"
#include "errors.hpp"
#include "rnd.hpp"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <unistd.h>


class FileThrashState
{
public:
  FileThrashState()
    : fd(-1),
      size(0),
      is_finished(false)
  {
  }

public:
  int fd;
  size_t size;
  bool is_finished;
};

namespace l
{
  static
  int
  nprocs(void)
  {
#ifdef _SC_NPROCESSORS_ONLN
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    return 4;
#endif
  }

  static
  void
  disable_signals(void)
  {
    sigset_t sigset;

    sigfillset(&sigset);
    pthread_sigmask(SIG_SETMASK,&sigset,NULL);
  }

  static
  void*
  write_thread(void *data_)
  {
    FileThrashState *state = (FileThrashState*)data_;
    ssize_t rv;
    off_t offset;
    std::vector<uint8_t> buf;

    buf.resize(1024*1024,0xFF);
    while(!state->is_finished)
      {
        offset = (std::rand() % state->size);

        rv = pwrite(state->fd,&buf[0],buf.size(),offset);
        if(rv == -1)
          printf("pwrite error: %d - %s\n",errno,strerror(errno));

      }

    return NULL;
  }

  static
  void*
  read_thread(void *data_)
  {
    FileThrashState *state = (FileThrashState*)data_;
    ssize_t rv;
    off_t offset;
    std::vector<uint8_t> buf;

    buf.resize(1024*1024);
    while(!state->is_finished)
      {
        offset = (std::rand() % state->size);

        rv = pread(state->fd,&buf[0],buf.size(),offset);
        if(rv == -1)
          printf("pread error: %d - %s\n",errno,strerror(errno));
      }

    return NULL;
  }

  static
  void
  spawn_thread(void*                  (*func_)(void*),
               FileThrashState        *state_,
               std::vector<pthread_t> *threads_)
  {
    int rv;
    sigset_t sigset_old;
    sigset_t sigset_new;
    pthread_t thread_id;

    sigfillset(&sigset_new);
    pthread_sigmask(SIG_SETMASK,&sigset_new,&sigset_old);

    rv = pthread_create(&thread_id,NULL,func_,state_);
    if(rv == 0)
      threads_->push_back(thread_id);

    pthread_sigmask(SIG_SETMASK,&sigset_old,NULL);
  }
}

#ifndef FALLOC_FL_ZERO_RANGE
#define FALLOC_FL_ZERO_RANGE 0
#endif

namespace bbf
{
  AppError
  filethrash(const Options &opts_)
  {
    int rv;
    int cores;
    FileThrashState state;
    struct statfs vfs;
    std::vector<pthread_t> threads;

    printf("Creating file: %s\n",opts_.device.c_str());
    rv = open(opts_.device.c_str(),O_RDWR|O_CREAT|O_SYNC|O_EXCL,0666);
    if(rv == -1)
      return AppError::opening_file(errno,opts_.device);

    state.fd = rv;

    rv = fstatfs(rv,&vfs);
    if(rv == -1)
      goto error;

    state.size = (vfs.f_bavail * vfs.f_frsize);

    printf("Expanding file to fill drive: %lu bytes\n",state.size);
    rv = fallocate(state.fd,FALLOC_FL_ZERO_RANGE,0,state.size);
    if(rv == -1)
      goto error;

    cores = l::nprocs();
    printf("Spawning thrashing threads: %d (one per core)\n",cores);
    for(int i = 0; i < cores; i++)
      l::spawn_thread(l::write_thread,&state,&threads);

    printf("CTRL-C to exit...\n");
    pause();

    l::disable_signals();
    state.is_finished = true;
    for(size_t i = 0; i < threads.size(); i++)
      pthread_join(threads[i],NULL);

    close(state.fd);
    unlink(opts_.device.c_str());

    return AppError::success();

  error:
    close(state.fd);
    unlink(opts_.device.c_str());
    return AppError::opening_file(errno,opts_.device);
  }
}
