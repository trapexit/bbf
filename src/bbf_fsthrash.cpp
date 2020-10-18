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

#include <algorithm>
#include <vector>
#include <string>
#include <utility>

#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILES 1024


class FSThrashState
{
public:
  FSThrashState(const std::string &basedir_)
    : is_finished(false),
      verbose(false),
      basedir(basedir_),
      mutex_dir(PTHREAD_MUTEX_INITIALIZER),
      mutex_file(PTHREAD_MUTEX_INITIALIZER),
      mutex_fd(PTHREAD_MUTEX_INITIALIZER)
  {

  }

public:
  std::string
  rnd_dir(void)
  {
    std::string rv;

    pthread_mutex_lock(&mutex_dir);
    if(!dirs.empty())
      rv = rnd::element(dirs);
    pthread_mutex_unlock(&mutex_dir);

    return rv;
  }

  std::string
  rnd_file(void)
  {
    std::string rv;

    pthread_mutex_lock(&mutex_file);
    if(!files.empty())
      rv = rnd::element(files);
    pthread_mutex_unlock(&mutex_file);

    return rv;
  }

  int
  rnd_fd(void)
  {
    int rv = -1;

    pthread_mutex_lock(&mutex_fd);
    if(!fds.empty())
      rv = rnd::element(fds);
    pthread_mutex_unlock(&mutex_fd);

    return rv;
  }

  std::string
  rnd_path(void)
  {
    return basedir + '/' + \
      rnd::filename();
  }

  std::string
  rm_rnd_file(void)
  {
    size_t r;
    std::string rv;

    pthread_mutex_lock(&mutex_file);
    if(!files.empty())
      {
        r = (std::rand() % files.size());
        rv = files[r];
        std::swap(files[r],files[files.size()-1]);
        files.pop_back();
      }
    pthread_mutex_unlock(&mutex_file);

    return rv;
  }

  std::string
  rm_rnd_dir(void)
  {
    size_t r;
    std::string rv;

    pthread_mutex_lock(&mutex_dir);
    if(!dirs.empty())
      {
        r = (std::rand() % dirs.size());
        rv = dirs[r];
        std::swap(dirs[r],dirs[dirs.size()-1]);
        dirs.pop_back();
      }
    pthread_mutex_unlock(&mutex_dir);

    return rv;
  }

  int
  rm_rnd_fd(void)
  {
    int rv;
    size_t r;

    rv = -1;
    pthread_mutex_lock(&mutex_fd);
    if(!fds.empty())
      {
        r = (std::rand() % fds.size());
        rv = fds[r];
        std::swap(fds[r],fds[fds.size()-1]);
        fds.pop_back();
      }
    pthread_mutex_unlock(&mutex_fd);

    return rv;
  }

public:
  void
  add_dir(const std::string &dir_)
  {
    pthread_mutex_lock(&mutex_dir);
    dirs.push_back(dir_);
    pthread_mutex_unlock(&mutex_dir);
  }

  void
  add_file(const std::string &file_)
  {
    pthread_mutex_lock(&mutex_file);
    files.push_back(file_);
    pthread_mutex_unlock(&mutex_file);
  }

  void
  add_fd(const int fd_)
  {
    pthread_mutex_lock(&mutex_fd);
    fds.push_back(fd_);
    pthread_mutex_unlock(&mutex_fd);
  }

public:
  bool is_finished;
  bool verbose;
  std::string basedir;

private:
  std::vector<std::string> files;
  std::vector<std::string> dirs;
  std::vector<int>         fds;

private:
  mutable pthread_mutex_t mutex_dir;
  mutable pthread_mutex_t mutex_file;
  mutable pthread_mutex_t mutex_fd;
};

namespace l
{
  static
  void
  millisleep(int ms_)
  {
    struct timespec ts;

    ts.tv_sec  = 0;
    ts.tv_nsec = (1000000 * ms_);

    nanosleep(&ts,NULL);
  }

  static
  void*
  create_thread(void* data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    std::string filepath;

    while(!state->is_finished)
      {
        filepath = state->rnd_path();

        rv = open(filepath.c_str(),O_RDWR|O_CREAT|O_APPEND|O_SYNC|O_EXCL,0666);
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("create: %s\n",filepath.c_str());

        state->add_fd(rv);
        state->add_file(filepath);

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  open_thread(void* data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    std::string filepath;

    while(!state->is_finished)
      {
        filepath = state->rnd_file();

        rv = open(filepath.c_str(),O_RDWR|O_APPEND|O_SYNC,0666);
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("open: %s\n",filepath.c_str());

        state->add_fd(rv);

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  close_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int fd;
    int rv;

    while(!state->is_finished)
      {
        l::millisleep(10);

        fd = state->rm_rnd_fd();
        if(fd == -1)
          goto error;

        rv = close(fd);
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("close: %d\n",fd);

        continue;

      error:
        l::millisleep(100);
      }

    fd = state->rm_rnd_fd();
    while(fd > 0)
      {
        close(fd);
        fd = state->rm_rnd_fd();
      }

    return NULL;
  }

  static
  void*
  mkdir_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    std::string dirpath;

    while(!state->is_finished)
      {
        l::millisleep(10);

        dirpath = state->rnd_path();

        rv = mkdir(dirpath.c_str(),0666);
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("mkdir: %s\n",dirpath.c_str());

        state->add_dir(dirpath);

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  write_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int fd;
    ssize_t rv;
    std::vector<uint8_t> buf;

    buf.resize(1024 * 1024);
    while(!state->is_finished)
      {
        fd = state->rnd_fd();
        if(fd == -1)
          goto error;

        rv = write(fd,&buf[0],buf.size());
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("write: fd = %d; size = %lu\n",fd,rv);

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  read_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int fd;
    ssize_t rv;
    off_t offset;
    struct stat st;
    std::vector<uint8_t> buf;

    buf.resize(1024 * 1024);
    while(!state->is_finished)
      {
        fd = state->rnd_fd();
        if(fd == -1)
          goto error;

        rv = fstat(fd,&st);
        if((rv == -1) || (st.st_size == 0))
          goto error;

        offset = (std::rand() % st.st_size);
        rv = pread(fd,&buf[0],buf.size(),offset);
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("read: fd = %d; size = %lu; offset = %ld\n",fd,rv,offset);

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  unlink_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    std::string filepath;

    while(!state->is_finished)
      {
        l::millisleep(10);

        filepath = state->rm_rnd_file();
        if(filepath.empty())
          goto error;

        rv = unlink(filepath.c_str());
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("unlink: %s\n",filepath.c_str());

        continue;

      error:
        l::millisleep(100);
      }

    filepath = state->rm_rnd_file();
    while(!filepath.empty())
      {
        unlink(filepath.c_str());
        filepath = state->rm_rnd_file();
      }

    return NULL;
  }

  static
  void*
  readdir_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    DIR *d;
    unsigned int count;
    struct dirent *dent;

    while(!state->is_finished)
      {
        l::millisleep(10);

        d = opendir(state->basedir.c_str());
        if(d == NULL)
          goto error;

        count = 0;
        for(dent = readdir(d); dent != NULL; dent = readdir(d))
          {

          }

        closedir(d);

        if(state->verbose)
          printf("readdir: count = %u\n",count);

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  stat_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    struct stat st;
    std::string filepath;

    while(!state->is_finished)
      {
        l::millisleep(10);

        filepath = state->rnd_file();
        if(filepath.empty())
          goto error;

        rv = stat(filepath.c_str(),&st);
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("stat: %s\n",filepath.c_str());

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  chmod_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    std::string filepath;

    while(!state->is_finished)
      {
        l::millisleep(10);

        filepath = state->rnd_file();
        if(filepath.empty())
          goto error;

        rv = chmod(filepath.c_str(),0666);
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("chmod: %s\n",filepath.c_str());

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  chown_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    uid_t uid;
    gid_t gid;
    std::string filepath;

    uid = geteuid();
    gid = getegid();
    while(!state->is_finished)
      {
        l::millisleep(10);

        filepath = state->rnd_file();
        if(filepath.empty())
          goto error;

        rv = chown(filepath.c_str(),uid,gid);
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("chown: %s\n",filepath.c_str());

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  link_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    std::string oldpath;
    std::string newpath;

    while(!state->is_finished)
      {
        l::millisleep(10);

        oldpath = state->rnd_file();
        if(oldpath.empty())
          goto error;

        newpath = state->rnd_path();

        rv = link(oldpath.c_str(),newpath.c_str());
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("link: %s -> %s\n",oldpath.c_str(),newpath.c_str());

        state->add_file(newpath);

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  symlink_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    std::string linkpath;
    std::string target;

    while(!state->is_finished)
      {
        l::millisleep(10);

        target = state->rnd_file();
        if(target.empty())
          goto error;

        linkpath = state->rnd_path();

        rv = symlink(target.c_str(),linkpath.c_str());
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("symlink: %s -> %s\n",target.c_str(),linkpath.c_str());

        state->add_file(linkpath);

        continue;

      error:
        l::millisleep(100);
      }

    return NULL;
  }

  static
  void*
  rmdir_thread(void *data_)
  {
    FSThrashState *state = (FSThrashState*)data_;
    int rv;
    std::string dirpath;

    while(!state->is_finished)
      {
        l::millisleep(10);

        dirpath = state->rm_rnd_dir();
        if(dirpath.empty())
          goto error;

        rv = rmdir(dirpath.c_str());
        if(rv == -1)
          goto error;

        if(state->verbose)
          printf("rmdir: %s\n",dirpath.c_str());

        continue;

      error:
        l::millisleep(100);
      }

    dirpath = state->rm_rnd_dir();
    while(!dirpath.empty())
      {
        rmdir(dirpath.c_str());
        dirpath = state->rm_rnd_dir();
      }

    return NULL;
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
  void
  spawn_thread(void*                  (*func_)(void*),
               FSThrashState           *state_,
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

  AppError
  fsthrash(const Options &opts_)
  {
    FSThrashState state(opts_.device);
    int rv;
    pthread_t thread_id;
    std::vector<pthread_t> threads;

    state.verbose = !opts_.quiet;
    l::spawn_thread(l::create_thread,&state,&threads);
    l::spawn_thread(l::open_thread,&state,&threads);
    l::spawn_thread(l::mkdir_thread,&state,&threads);
    l::spawn_thread(l::write_thread,&state,&threads);
    l::spawn_thread(l::read_thread,&state,&threads);
    l::spawn_thread(l::unlink_thread,&state,&threads);
    l::spawn_thread(l::rmdir_thread,&state,&threads);
    l::spawn_thread(l::close_thread,&state,&threads);
    l::spawn_thread(l::readdir_thread,&state,&threads);
    l::spawn_thread(l::stat_thread,&state,&threads);
    l::spawn_thread(l::chmod_thread,&state,&threads);
    l::spawn_thread(l::chown_thread,&state,&threads);
    l::spawn_thread(l::link_thread,&state,&threads);
    l::spawn_thread(l::symlink_thread,&state,&threads);

    printf("CTRL-C to exit...\n");
    pause();
    printf("Cleaning up...\n");

    l::disable_signals();
    state.is_finished = true;
    for(size_t i = 0; i < threads.size(); i++)
      pthread_join(threads[i],NULL);

    return AppError::success();
  }
}

namespace bbf
{
  AppError
  fsthrash(const Options &opts_)
  {
    int rv;
    struct stat st;

    rv = stat(opts_.device.c_str(),&st);
    if(rv == -1)
      return AppError::opening_file(errno,opts_.device);
    if(!S_ISDIR(st.st_mode))
      return AppError::not_a_directory(opts_.device);

    return l::fsthrash(opts_);
  }
}
