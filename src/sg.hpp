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

namespace StatusCode
{
  enum
    {
      GOOD                 = (0x00 << 1),
      CHECK_CONDITION      = (0x01 << 1),
      CONDITION_GOOD       = (0x02 << 1),
      BUSY                 = (0x04 << 1),
      INTERMEDIATE_GOOD    = (0x08 << 1),
      INTERMEDIATE_C_GOOD  = (0x0a << 1),
      RESERVATION_CONFLICT = (0x0c << 1)
    };
}

namespace HostCode
{
  enum
    {
      OK         = 0x00000100,
      NO_CONNECT = 0x00000101,
      BUS_BUSY   = 0x00000102,
      TIME_OUT   = 0x00000103,
      BAD_TARGET = 0x00000104,
      ABORT      = 0x00000105,
      PARITY     = 0x00000106,
      ERROR      = 0x00000107,
      RESET      = 0x00000108,
      BAD_INTR   = 0x00000109
    };

  const char* to_string(const int err);
  const int   to_errno(const int err);
}

namespace DriverCode
{
  enum
    {
      OK      = 0x00000200,
      BUSY    = 0x00000201,
      SOFT    = 0x00000202,
      MEDIA   = 0x00000203,
      ERROR   = 0x00000204,
      INVALID = 0x00000205,
      TIMEOUT = 0x00000206,
      HARD    = 0x00000207,
      SENSE   = 0x00000208
    };

  const char* to_string(const int err);
  const int   to_errno(const int err);
}

namespace sg
{
  enum
    {
      ATA_USING_LBA = (1 << 6),
      ATA_STAT_DRQ  = (1 << 3),
      ATA_STAT_ERR  = (1 << 0)
    };

  /*
    7.71.2.2 Pseudo Uncorrectable Logical Sectors

    If the FEATURE field (7:0) contains a value of 55h, the WRITE
    UNCORRECTABLE EXT command shall cause the device to indicate a
    failure when subsequent reads to any of the logical sectors that
    are contained in the physical block of the specified logical
    sector are performed. These logical sectors are referred to as
    pseudo uncorrectable logical sectors. Whenever a pseudo
    uncorrectable logical sector is accessed via a read command the
    device shall perform normal error recovery to the fullest extent
    until:

    a) the error recovery process is completed, the UNCORRECTABLEERROR
    bit is set to one, and the ERROR bit is set to one; or
    b) a command time-out that applies to error recovery control
    occurs before error recovery is completed and an error is reported
    as a result of the command time-out (see 8.3.3). As part of
    reading a pseudo uncorrectable logical sector, the device shall
    perform error logging (e.g., SMART, device statistics) in the same
    manner as an Uncorrectable error (see 6.3.9).

    7.71.2.3 Flagged Uncorrectable Logical Sectors

    If the FEATURE field (7:0) contains a value of AAh, the WRITE
    UNCORRECTABLE EXT command shall cause the device to mark the
    specified logical sectors as flagged uncorrectable. Marking a
    logical sector as flagged uncorrectable shall cause the device to
    indicate a failure when subsequent reads to the specified logical
    sector are processed.As part of reading a flagged uncorrectable
    logical sector, the device should not perform error logging (e.g.,
    SMART, device statistics) in the same manner as an Uncorrectable
    error (see 6.3.9)
  */

  enum
    {
      ATA_WRITE_UNCORRECTABLE_EXT_PSEUDO_W_LOGGING   = 0x55,
      ATA_WRITE_UNCORRECTABLE_EXT_PSEUDO_WO_LOGGING  = 0x5A,
      ATA_WRITE_UNCORRECTABLE_EXT_FLAGGED_W_LOGGING  = 0xA5,
      ATA_WRITE_UNCORRECTABLE_EXT_FLAGGED_WO_LOGGING = 0xAA
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
      ATA_OP_SECURITY_SET_PASSWORD  = 0xf1,
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

  enum
    {
      SG_IDENTIFIER_USER   = 0,
      SG_IDENTIFIER_MASTER = 1
    };

  enum
    {
      SG_ERASE_NORMAL   = 0,
      SG_ERASE_ENHANCED = 1
    };

  enum
    {
      FORM_FACTOR_UNKNOWN = 0x00,
      FORM_FACTOR_5_25    = 0x01,
      FORM_FACTOR_3_5     = 0x02,
      FORM_FACTOR_2_5     = 0x03,
      FORM_FACTOR_1_8     = 0x04,
      FORM_FACTOR_LT_1_8  = 0x05
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
    uint64_t write_uncorrectable:1;
    uint64_t smart_supported:1;
    uint64_t smart_enabled:1;
    uint64_t security_supported:1;
    uint64_t security_enabled:1;
    uint64_t security_locked:1;
    uint64_t security_frozen:1;
    uint64_t security_count_expired:1;
    uint64_t security_enhanced_erase_supported:1;
    uint64_t block_erase:1;
    uint64_t overwrite:1;
    uint64_t crypto_scramble:1;
    uint64_t sanitize:1;
    uint64_t supports_sata_gen1:1;
    uint64_t supports_sata_gen2:1;
    uint64_t supports_sata_gen3:1;
    uint64_t trim_supported:1;

    uint8_t  form_factor;
    uint32_t rpm;
    uint32_t security_normal_erase_time;
    uint32_t security_enhanced_erase_time;

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
             const uint64_t  blocks,
             void           *buf,
             const size_t    buflen,
             const int       timeout);

  int
  write_block(const int       fd,
              const uint64_t  lba,
              const uint64_t  blocks,
              const void     *buf,
              const size_t    buflen,
              const int       timeout);

  int
  write_uncorrectable(const int      fd,
                      const uint64_t lba,
                      const uint8_t  type,
                      const int      timeout);

  int
  write_flagged_uncorrectable(const int      fd_,
                              const uint64_t lba_,
                              const bool     log_,
                              const int      timeout_);
  int
  write_pseudo_uncorrectable(const int      fd_,
                             const uint64_t lba_,
                             const bool     log_,
                             const int      timeout_);

  int
  security_set_password(const int  fd,
                        const int  identifier,
                        const char password[32],
                        const int  timeout);
  int
  security_erase_prepare(const int fd,
                         const int timeout);
  int
  security_erase(const int  fd,
                 const int  erase_mode,
                 const int  identifier,
                 const char password[32],
                 const int  timeout);
}
