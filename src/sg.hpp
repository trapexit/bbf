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

#ifndef __SG_HPP__
#define __SG_HPP__

#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>

#define LBA28_LIMIT ((uint64_t)(1<<28)-1)

#define SG_READ  0
#define SG_WRITE 1

#define SG_PIO 0
#define SG_DMA 1

#define SG_ATA_12		0xa1
#define SG_ATA_12_LEN		12
#define SG_ATA_16		0x85
#define SG_ATA_16_LEN           16
#define SG_ATA_LBA48		1
#define SG_ATA_PROTO_NON_DATA	(3 << 1)
#define SG_ATA_PROTO_PIO_IN	(4 << 1)
#define SG_ATA_PROTO_PIO_OUT	(5 << 1)
#define SG_ATA_PROTO_DMA	(6 << 1)

#define SG_CHECK_CONDITION 0x02
#define SG_DRIVER_SENSE    0x08

namespace sg
{
  enum
    {
      ATA_USING_LBA = (1 << 6),
      ATA_STAT_DRQ  = (1 << 3),
      ATA_STAT_ERR  = (1 << 0)
    };

  enum
    {
      ATA_WRITE_UNCORRECTABLE_EXT_PSEUDO  = 0x55,
      ATA_WRITE_UNCORRECTABLE_EXT_FLAGGED = 0xAA
    };

  enum
    {
      ATA_OP_DSM                    = 0x06,
      ATA_OP_READ_PIO               = 0x20,
      ATA_OP_READ_PIO_ONCE          = 0x21,
      ATA_OP_READ_LONG              = 0x22,
      ATA_OP_READ_LONG_ONCE         = 0x23,
      ATA_OP_READ_PIO_EXT           = 0x24,
      ATA_OP_READ_DMA_EXT           = 0x25,
      ATA_OP_READ_LOG_EXT           = 0x2f,
      ATA_OP_READ_FPDMA             = 0x60,
      ATA_OP_WRITE_PIO              = 0x30,
      ATA_OP_WRITE_LONG             = 0x32,
      ATA_OP_WRITE_LONG_ONCE        = 0x33,
      ATA_OP_WRITE_PIO_EXT          = 0x34,
      ATA_OP_WRITE_DMA_EXT          = 0x35,
      ATA_OP_WRITE_FPDMA            = 0x61,
      ATA_OP_READ_VERIFY            = 0x40,
      ATA_OP_READ_VERIFY_ONCE       = 0x41,
      ATA_OP_READ_VERIFY_EXT        = 0x42,
      ATA_OP_WRITE_UNC_EXT          = 0x45,
      ATA_OP_FORMAT_TRACK           = 0x50,
      ATA_OP_DOWNLOAD_MICROCODE     = 0x92,
      ATA_OP_STANDBYNOW2            = 0x94,
      ATA_OP_CHECKPOWERMODE2        = 0x98,
      ATA_OP_SLEEPNOW2              = 0x99,
      ATA_OP_PIDENTIFY              = 0xa1,
      ATA_OP_READ_NATIVE_MAX        = 0xf8,
      ATA_OP_READ_NATIVE_MAX_EXT    = 0x27,
      ATA_OP_SMART                  = 0xb0,
      ATA_OP_DCO                    = 0xb1,
      ATA_OP_ERASE_SECTORS          = 0xc0,
      ATA_OP_READ_DMA               = 0xc8,
      ATA_OP_WRITE_DMA              = 0xca,
      ATA_OP_DOORLOCK               = 0xde,
      ATA_OP_DOORUNLOCK             = 0xdf,
      ATA_OP_STANDBYNOW1            = 0xe0,
      ATA_OP_IDLEIMMEDIATE          = 0xe1,
      ATA_OP_SETIDLE                = 0xe3,
      ATA_OP_SET_MAX                = 0xf9,
      ATA_OP_SET_MAX_EXT            = 0x37,
      ATA_OP_SET_MULTIPLE           = 0xc6,
      ATA_OP_CHECKPOWERMODE1        = 0xe5,
      ATA_OP_SLEEPNOW1              = 0xe6,
      ATA_OP_FLUSHCACHE             = 0xe7,
      ATA_OP_FLUSHCACHE_EXT         = 0xea,
      ATA_OP_IDENTIFY               = 0xec,
      ATA_OP_SETFEATURES            = 0xef,
      ATA_OP_SECURITY_SET_PASS      = 0xf1,
      ATA_OP_SECURITY_UNLOCK        = 0xf2,
      ATA_OP_SECURITY_ERASE_PREPARE = 0xf3,
      ATA_OP_SECURITY_ERASE_UNIT    = 0xf4,
      ATA_OP_SECURITY_FREEZE_LOCK   = 0xf5,
      ATA_OP_SECURITY_DISABLE       = 0xf6,
      ATA_OP_VENDOR_SPECIFIC_0x80   = 0x80,
    };

  enum
    {
      SG_CDB2_TLEN_NODATA   = 0 << 0,
      SG_CDB2_TLEN_FEAT     = 1 << 0,
      SG_CDB2_TLEN_NSECT    = 2 << 0,
      SG_CDB2_TLEN_BYTES    = 0 << 2,
      SG_CDB2_TLEN_SECTORS  = 1 << 2,
      SG_CDB2_TDIR_TO_DEV   = 0 << 3,
      SG_CDB2_TDIR_FROM_DEV = 1 << 3,
      SG_CDB2_CHECK_COND    = 1 << 5,
    };

  struct ata_lba_regs
  {
    uint8_t feat;
    uint8_t nsect;
    uint8_t lbal;
    uint8_t lbam;
    uint8_t lbah;
  };

  struct ata_tf
  {
    uint8_t             dev;
    uint8_t             command;
    uint8_t             error;
    uint8_t             status;
    uint8_t             is_lba48;
    struct ata_lba_regs lob;
    struct ata_lba_regs hob;
  };

  struct identity
  {
    unsigned int write_uncorrectable_ext:1;
    unsigned int smart_supported:1;
    unsigned int smart_enabled:1;

    int rpm;

    char serial_number[20+1];
    char firmware_revision[8+1];
    char model_number[40+1];
  };

  int
  exec(const int            fd,
       const int            rw,
       const int            dma,
       const struct ata_tf *tf,
       void                *data,
       const unsigned int   data_bytes,
       const unsigned int   timeout);

  int
  identify(const int     fd,
           sg::identity &ident);

  int
  flush_write_cache(const int fd,
                    const int timeout);

  int
  read_block(const int       fd,
             const uint64_t  lba,
             const uint64_t  logical_block_size,
             void           *buf,
             const size_t    buflen,
             const int       timeout);

  int
  write_block(const int       fd,
              const uint64_t  lba,
              const uint64_t  logical_block_size,
              const void     *buf,
              const size_t    buflen,
              const int       timeout);

  int
  write_uncorrectable(const int      fd,
                      const uint64_t lba,
                      const uint8_t  type,
                      const int      timeout);

  int
  write_flagged_uncorrectable(const int      fd,
                              const uint64_t lba,
                              const int      timeout);
  int
  write_pseudo_uncorrectable(const int      fd,
                             const uint64_t lba,
                             const int      timeout);
}

#endif
