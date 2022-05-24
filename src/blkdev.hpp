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

#pragma once

#include "sg.hpp"

#include <string>
#include <vector>

#include <stdlib.h>
#include <stdint.h>


class BlkDev
{
public:
  BlkDev();
  ~BlkDev();

private:
  void _reset_data(void);

public:
  int open(const std::string &path,
           const int          flags);
  int open_read(const std::string &path);
  int open_rdwr(const std::string &path,
                const bool         excl = true);
  int close(void);

public:
  int64_t os_read(const uint64_t  lba,
                  const uint64_t  blocks,
                  void           *buf,
                  const uint64_t  buflen);

  int64_t os_write(const uint64_t  lba,
                   const uint64_t  blocks,
                   const void     *buf,
                   const uint64_t  buflen);

  int64_t ata_read(const uint64_t  lba,
                   const uint64_t  blocks,
                   void           *buf,
                   const uint64_t  buflen);

  int64_t ata_write(const uint64_t  lba,
                    const uint64_t  blocks,
                    const void     *buf,
                    const uint64_t  buflen);

private:
  enum RWType
    {
      ATA,
      OS
    };

  RWType _rw_type;

public:
  void set_rw_ata(void) { _rw_type = ATA; }
  void set_rw_os(void)  { _rw_type = OS;  }
  int64_t read(const uint64_t  lba,
               const uint64_t  blocks,
               void           *buf,
               const uint64_t  buflen);

  int64_t read(const uint64_t     lba,
               const uint64_t     blocks,
               std::vector<char> &buf);

  int64_t write(const uint64_t  lba,
                const uint64_t  blocks,
                const void     *buf,
                const uint64_t  buflen);

  int64_t write(const uint64_t           lba,
                const uint64_t           blocks,
                const std::vector<char> &buf);

public:
  int sync(void);
  int write_flagged_uncorrectable(const uint64_t lba_,
                                  const bool     log_);
  int write_pseudo_uncorrectable(const uint64_t lba_,
                                 const bool     log_);

public:
  uint64_t logical_block_size(void) const { return _logical_block_size; }
  uint64_t physical_block_size(void) const { return _physical_block_size; }
  uint64_t size_in_bytes(void) const { return _size_in_bytes; }
  uint64_t logical_block_count(void) const { return _logical_block_count; }
  uint64_t physical_block_count(void) const { return _physical_block_count; }
  uint64_t block_stepping(void) const;

public:
  const bool  has_identity(void) const { return _has_identity; }
  const sg::identity &identity() const { return _identity; }

public:
  int fd(void) const { return _fd; }

private:
  uint64_t _logical_block_size;
  uint64_t _physical_block_size;
  uint64_t _size_in_bytes;
  uint64_t _logical_block_count;
  uint64_t _physical_block_count;

private:
  sg::identity _identity;
  bool         _has_identity;

private:
  int _fd;
  int _timeout;
};
