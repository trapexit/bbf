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

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "blkdev.hpp"
#include "ioctl.hpp"

#define SECONDS(x) ((x) * 1000)

static
int
_block_device(const int fd)
{
  int rv;
  struct stat st;

  rv = ::fstat(fd,&st);
  if(rv == -1)
    return -errno;

  if(!S_ISBLK(st.st_mode))
    return -ENOTBLK;

  return 0;
}

void
BlkDev::_reset_data(void)
{
  _fd                   = -1;
  _logical_block_size   =  0;
  _physical_block_size  =  0;
  _size_in_bytes        =  0;
  _logical_block_count  =  0;
  _physical_block_count =  0;
  _timeout              =  SECONDS(60);
  _has_identity         =  false;
}

BlkDev::BlkDev()
  : _rw_type(OS)
{
  _reset_data();
}

BlkDev::~BlkDev()
{
  if(_fd != -1)
    ::close(_fd);
}

int
BlkDev::open_read(const std::string &path)
{
  const int flags = O_RDONLY|O_NONBLOCK;

  return open(path,flags);
}

int
BlkDev::open_rdwr(const std::string &path,
                  const bool         excl)
{
  int flags = O_RDWR|O_NONBLOCK;

  flags |= (excl ? O_EXCL : 0);

  return open(path,flags);
}

int
BlkDev::open(const std::string &path,
             const int          flags)
{
  int64_t rv;

  _fd = ::open(path.c_str(),flags);
  if(_fd == -1)
    return -errno;

  rv = _block_device(_fd);
  if(rv < 0)
    goto error;

  rv = sg::identify(_fd,_identity);
  if(rv == 0)
    _has_identity = true;

  rv = IOCtl::logical_block_size(_fd);
  if(rv < 0)
    goto error;
  _logical_block_size = rv;

  rv = IOCtl::physical_block_size(_fd);
  if(rv < 0)
    goto error;
  _physical_block_size = rv;

  rv = IOCtl::size_in_bytes(_fd);
  if(rv < 0)
    goto error;
  _size_in_bytes = rv;

  _logical_block_count  = (_size_in_bytes / _logical_block_size);
  _physical_block_count = (_size_in_bytes / _physical_block_size);

  return 0;

 error:
  ::close(_fd);

  _reset_data();

  return rv;
}

int
BlkDev::close(void)
{
  int rv;

  if(_fd == -1)
    return 0;

  rv = ::close(_fd);

  _reset_data();

  return ((rv == -1) ? -errno : rv);
}

ssize_t
BlkDev::os_read(const uint64_t  lba,
                void           *buf,
                const size_t    buflen)
{
  ssize_t rv;
  off_t offset;

  offset = (lba * _logical_block_size);

  rv = ::pread(_fd,buf,buflen,offset);

  return ((rv == -1) ? -errno : rv);
}

ssize_t
BlkDev::os_write(const uint64_t  lba,
                 const void     *buf,
                 const size_t    buflen)
{
  ssize_t rv;
  off_t offset;

  offset = (lba * _logical_block_size);

  rv = ::pwrite(_fd,buf,buflen,offset);

  return ((rv == -1) ? -errno : rv);
}

ssize_t
BlkDev::ata_read(const uint64_t  lba,
                 void           *buf,
                 const size_t    buflen)
{
  int rv;

  rv = sg::read_block(_fd,lba,
                      _logical_block_size,
                      buf,buflen,
                      _timeout);
  if(rv < 0)
    return rv;

  return buflen;
}

ssize_t
BlkDev::ata_write(const uint64_t  lba,
                  const void     *buf,
                  const size_t    buflen)
{
  int rv;

  rv = sg::write_block(_fd,lba,
                       _logical_block_size,
                       buf,buflen,
                       _timeout);
  if(rv < 0)
    return rv;

  return buflen;
}

ssize_t
BlkDev::read(const uint64_t  lba,
             void           *buf,
             const size_t    buflen)
{
  switch(_rw_type)
    {
    case ATA:
      return ata_read(lba,buf,buflen);
    case OS:
      return os_read(lba,buf,buflen);
    }

  return -ENOTSUP;
}

ssize_t
BlkDev::write(const uint64_t  lba,
              const void     *buf,
              const size_t    buflen)
{
  switch(_rw_type)
    {
    case ATA:
      return ata_write(lba,buf,buflen);
    case OS:
      return os_write(lba,buf,buflen);
    }

  return -ENOTSUP;
}

uint64_t
BlkDev::block_stepping(void) const
{
  return (_physical_block_size / _logical_block_size);
}

int
BlkDev::sync(void)
{
  int rv;
  int error;

  error = 0;

  rv = ::fsync(_fd);
  if(rv == -1)
    error = -errno;

  rv = ::fdatasync(_fd);
  if(rv == -1)
    error = -errno;

  rv = IOCtl::block_flush(_fd);
  if(rv < 0)
    error = rv;

  return error;
}

int
BlkDev::write_flagged_uncorrectable(const uint64_t lba)
{
  return sg::write_flagged_uncorrectable(_fd,lba,_timeout);
}

int
BlkDev::write_pseudo_uncorrectable(const uint64_t lba)
{
  return sg::write_pseudo_uncorrectable(_fd,lba,_timeout);
}

const
int
BlkDev::rpm(void) const
{
  return _identity.rpm;
}

const
bool
BlkDev::write_uncorrectable_ext(void) const
{
  return _identity.write_uncorrectable_ext;
}

const
bool
BlkDev::smart_supported(void) const
{
  return _identity.smart_supported;
}

const
bool
BlkDev::smart_enabled(void) const
{
  return _identity.smart_enabled;
}

const
char *
BlkDev::serial_number(void) const
{
  return _identity.serial_number;
}

const
char *
BlkDev::firmware_revision(void) const
{
  return _identity.firmware_revision;
}

const
char *
BlkDev::model_number(void) const
{
  return _identity.model_number;
}
