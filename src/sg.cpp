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

#include "sensedata.hpp"
#include "sg.hpp"

#include <errno.h>
#include <linux/hdreg.h>
#include <stddef.h>
#include <scsi/sg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static
inline
uint64_t
next_power_of_2(uint64_t v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v |= v >> 32;
  v++;

  return v;
}

namespace HostCode
{
  const
  char*
  to_string(const int err_)
  {
    switch(err_)
      {
      case OK:
        return "No error (0x00)";
      case NO_CONNECT:
        return "Couldn't connect before timeout period (0x01)";
      case BUS_BUSY:
        return "Bus stayed busy through time out period (0x02)";
      case TIME_OUT:
        return "Timed out for other reason (0x03)";
      case BAD_TARGET:
        return "Bad target (0x04)";
      case ABORT:
        return "Told to abort for some other reason (0x05)";
      case PARITY:
        return "Parity error (0x06)";
      case ERROR:
        return "Internal error (0x07)";
      case RESET:
        return "Reset by somebody (0x08)";
      case BAD_INTR:
        return "Got an interrupt we weren't expecting (0x09)";
      }

    return "invalid error";
  }

  const
  int
  to_errno(const int err_)
  {
    return (err_ | 0x00000100);
  }
}

namespace DriverCode
{
  const
  char*
  to_string(const int err_)
  {
    switch(err_)
      {
      case OK:
        return "No Error (0x00)";
      case BUSY:
        return "Busy (0x01)";
      case SOFT:
        return "Soft (0x02)";
      case MEDIA:
        return "Media (0x03)";
      case ERROR:
        return "internal driver error (0x04)";
      case INVALID:
        return "finished (DID_BAD_TARGET or DID_ABORT) (0x05)";
      case TIMEOUT:
        return "finished with timeout (0x06)";
      case HARD:
        return "finished with fatal error (0x07)";
      case SENSE:
        return "had sense information available (0x08)";
      }

    return "unknown";
  }

  const
  int
  to_errno(const int err_)
  {
    return (err_ | 0x00000200);
  }
}


namespace sg
{
  static
  bool
  needs_lba48(uint8_t      ata_op,
              uint64_t     lba,
              unsigned int nsect)
  {
    switch (ata_op)
      {
      case ATA_OP_DSM:
      case ATA_OP_READ_PIO_EXT:
      case ATA_OP_READ_DMA_EXT:
      case ATA_OP_WRITE_PIO_EXT:
      case ATA_OP_WRITE_DMA_EXT:
      case ATA_OP_READ_VERIFY_EXT:
      case ATA_OP_WRITE_UNC_EXT:
      case ATA_OP_READ_NATIVE_MAX_EXT:
      case ATA_OP_SET_MAX_EXT:
      case ATA_OP_FLUSHCACHE_EXT:
        return true;
      case ATA_OP_SECURITY_ERASE_PREPARE:
      case ATA_OP_SECURITY_ERASE_UNIT:
      case ATA_OP_VENDOR_SPECIFIC_0x80:
      case ATA_OP_SMART:
        return false;
      }

    if(lba >= LBA28_LIMIT)
      return true;

    if(nsect > 0xFF)
      return true;

    if((lba + nsect - 1) >= LBA28_LIMIT)
      return true;

    return false;
  }

  inline
  int
  is_dma(uint8_t ata_op)
  {
    switch (ata_op)
      {
      case ATA_OP_DSM:
      case ATA_OP_READ_DMA_EXT:
      case ATA_OP_READ_FPDMA:
      case ATA_OP_WRITE_DMA_EXT:
      case ATA_OP_WRITE_FPDMA:
      case ATA_OP_READ_DMA:
      case ATA_OP_WRITE_DMA:
        return SG_DMA;
      default:
        return SG_PIO;
      }
  }

  void
  tf_init(struct ata_tf *tf,
          uint8_t        ata_op,
          uint64_t       lba,
          unsigned int   nsect)
  {
    memset(tf,0,sizeof(struct ata_tf));

    tf->command   = ata_op;
    tf->dev       = ATA_USING_LBA | 0xA0;
    tf->lob.lbal  = lba;
    tf->lob.lbam  = lba >>  8;
    tf->lob.lbah  = lba >> 16;
    tf->lob.nsect = nsect;

    if(needs_lba48(ata_op,lba,nsect))
      {
        tf->is_lba48  = 1;
        tf->hob.feat  = 0;
        tf->hob.nsect = nsect >>  8;
        tf->hob.lbal  =   lba >> 24;
        tf->hob.lbam  =   lba >> 32;
        tf->hob.lbah  =   lba >> 40;
      }
    else
      {
        tf->dev |= (lba >> 24) & 0x0F;
      }
  }

  static
  uint64_t
  tf_to_lba(const struct ata_tf *tf)
  {
    uint32_t lbah;
    uint32_t lba24;
    uint64_t lba64;

    lba24 = (tf->lob.lbah << 16) | (tf->lob.lbam << 8) | (tf->lob.lbal);

    lbah = (tf->is_lba48 ?
            ((tf->hob.lbah << 16) | (tf->hob.lbam << 8) | (tf->hob.lbal)) :
            (tf->dev & 0x0f));

    lba64 = (((uint64_t)lbah) << 24) | (uint64_t)lba24;

    return lba64;
  }

  static
  char
  calculate_cdb1(const int   dma,
                 const int   rw,
                 const void *data)
  {
    if(dma && data)
      return SG_ATA_PROTO_DMA;
    if(dma && !data)
      return SG_ATA_PROTO_NON_DATA;

    if(data &&  rw)
      return SG_ATA_PROTO_PIO_OUT;
    if(data && !rw)
      return SG_ATA_PROTO_PIO_IN;

    return SG_ATA_PROTO_NON_DATA;
  }

  static
  char
  calculate_cdb2(const int   rw,
                 const void *data)
  {
    if(data && rw)
      return (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
    if(data && !rw)
      return (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);

    return SG_CDB2_CHECK_COND;
  }

  static
  int
  populate_cdb_from_tf_ata16(unsigned char        cdb[SG_ATA_16_LEN],
                             const struct ata_tf *tf)
  {
    cdb[ 0]  = SG_ATA_16;
    cdb[ 1] |= SG_ATA_LBA48;
    cdb[ 3]  = tf->hob.feat;
    cdb[ 4]  = tf->lob.feat;
    cdb[ 5]  = tf->hob.nsect;
    cdb[ 6]  = tf->lob.nsect;
    cdb[ 7]  = tf->hob.lbal;
    cdb[ 8]  = tf->lob.lbal;
    cdb[ 9]  = tf->hob.lbam;
    cdb[10]  = tf->lob.lbam;
    cdb[11]  = tf->hob.lbah;
    cdb[12]  = tf->lob.lbah;
    cdb[13]  = tf->dev;
    cdb[14]  = tf->command;

    return SG_ATA_16_LEN;
  }

  static
  int
  populate_cdb_from_tf_ata12(unsigned char        cdb[SG_ATA_16_LEN],
                             const struct ata_tf *tf)
  {
    cdb[ 0] = SG_ATA_12;
    cdb[ 3] = tf->lob.feat;
    cdb[ 4] = tf->lob.nsect;
    cdb[ 5] = tf->lob.lbal;
    cdb[ 6] = tf->lob.lbam;
    cdb[ 7] = tf->lob.lbah;
    cdb[ 8] = tf->dev;
    cdb[ 9] = tf->command;

    return SG_ATA_12_LEN;
  }

  static
  int
  populate_cdb_from_tf(unsigned char        cdb[SG_ATA_16_LEN],
                       const struct ata_tf *tf)
  {
    return (tf->is_lba48 ?
            populate_cdb_from_tf_ata16(cdb,tf) :
            populate_cdb_from_tf_ata12(cdb,tf));
  }

  static
  int
  dxfer_direction(const void *data,
                  const int   rw)
  {
    if(data)
      {
        if(rw)
          return SG_DXFER_TO_DEV;
        return SG_DXFER_FROM_DEV;
      }

    return SG_DXFER_NONE;
  }

  static
  void
  populate_io_hdr(sg_io_hdr_t         *io_hdr,
                  unsigned char        cdb[SG_ATA_16_LEN],
                  unsigned char        sb[32],
                  const struct ata_tf *tf,
                  void                *data,
                  const int            data_bytes,
                  const int            rw,
                  const int            timeout)
  {
    io_hdr->interface_id    = 'S';
    io_hdr->dxfer_direction = dxfer_direction(data,rw);
    io_hdr->dxferp          = data;
    io_hdr->dxfer_len       = (data ? data_bytes : 0);
    io_hdr->cmdp            = cdb;
    io_hdr->sbp             = sb;
    io_hdr->mx_sb_len       = 32;
    io_hdr->pack_id         = tf_to_lba(tf);
    io_hdr->timeout         = (timeout ? timeout : 1000);
  }

  static
  int
  exec_core(const int    fd,
            sg_io_hdr_t &io_hdr)
  {
    int rv;

    rv = ::ioctl(fd,SG_IO,&io_hdr);
    if(rv == -1)
      return -errno;

    if(io_hdr.status && (io_hdr.status != StatusCode::CHECK_CONDITION))
      return -EBADE;
    if(io_hdr.host_status)
      return -HostCode::to_errno(io_hdr.host_status);
    if(io_hdr.driver_status &&
       ((DriverCode::to_errno(io_hdr.driver_status) != DriverCode::SENSE)))
      return -DriverCode::to_errno(io_hdr.driver_status);
    if((SenseData::sense_key(io_hdr.sbp) != SenseData::SenseKey::NO_SENSE) &&
       (SenseData::sense_key(io_hdr.sbp) != SenseData::SenseKey::RECOVERED_ERROR))
      return -SenseData::asc_ascq_to_errno(io_hdr.sbp);

    return 0;
  }

  int
  exec(const int            fd,
       const int            rw,
       const int            dma,
       const struct ata_tf *tf,
       void                *data,
       const unsigned int   data_bytes,
       const unsigned int   timeout)
  {
    int rv;
    uint8_t cdb[SG_ATA_16_LEN] = {0};
    uint8_t sb[32]             = {0};
    sg_io_hdr_t io_hdr         = {0};

    if(data && data_bytes && !rw)
      memset(data,0,data_bytes);

    cdb[1]         = calculate_cdb1(dma,rw,data);
    cdb[2]         = calculate_cdb2(rw,data);
    io_hdr.cmd_len = populate_cdb_from_tf(cdb,tf);
    populate_io_hdr(&io_hdr,cdb,sb,tf,data,data_bytes,rw,timeout);

    return sg::exec_core(fd,io_hdr);
  }

  void
  buf_to_identity(const char    buf[256*2],
                  sg::identity &ident)
  {
    uint16_t tmpbuf[256];
    const uint16_t *buf16 = (const uint16_t*)buf;

    ::memset(&ident,0,sizeof(sg::identity));
    for(unsigned int i = 0; i < 256; i++)
      tmpbuf[i] = ntohs(buf16[i]);

    ident.rpm = buf16[217];
    ident.trim_supported = !!(buf16[169] & 0x0001);
    ident.form_factor    = (buf16[168] & 0x0003);
    ident.smart_supported    = (((buf16[83] & 0xC000) == 0x4000) && (buf16[82] & 0x0001));
    ident.smart_enabled      = (((buf16[87] & 0xC000) == 0x4000) && (buf16[85] & 0x0001));
    ident.security_supported = (((buf16[83] & 0xC000) == 0x4000) && (buf16[82] & 0x0002));
    ident.security_enabled   = (((buf16[87] & 0xC000) == 0x4000) && (buf16[85] & 0x0002));
    ident.security_locked                   = !!(buf16[128] & 0x0004);
    ident.security_frozen                   = !!(buf16[128] & 0x0008);
    ident.security_count_expired            = !!(buf16[128] & 0x0010);
    ident.security_enhanced_erase_supported = !!(buf16[128] & 0x0020);
    ident.write_uncorrectable = (((buf16[83]  & 0xC000) == 0x4000) &&
                                 ((buf16[86]  & 0x8000) == 0x8000) &&
                                 ((buf16[119] & 0xC004) == 0x4004) &&
                                 ((buf16[120] & 0xC004) == 0x4004));
    ident.block_erase        = !!(buf16[59] & 0x8000);
    ident.overwrite          = !!(buf16[59] & 0x4000);
    ident.crypto_scramble    = !!(buf16[59] & 0x2000);
    ident.sanitize           = !!(buf16[59] & 0x1000);
    ident.supports_sata_gen1 = !!(buf16[76] & 0x0002);
    ident.supports_sata_gen2 = !!(buf16[76] & 0x0004);
    ident.supports_sata_gen3 = !!(buf16[76] & 0x0008);

    ident.security_normal_erase_time   = (((buf16[89] & 0x8000) ?
                                           (buf16[89] & 0x7FFF) :
                                           (buf16[89] & 0x00FF)) * 2);
    if((ident.security_normal_erase_time == 0x7FFF) ||
       (ident.security_normal_erase_time == 0x00FF))
      ident.security_normal_erase_time += 90;
    ident.security_enhanced_erase_time = (((buf16[90] & 0x8000) ?
                                           (buf16[90] & 0x7FFF) :
                                           (buf16[90] & 0x00FF)) * 2);
    if((ident.security_enhanced_erase_time == 0x7FFF) ||
       (ident.security_enhanced_erase_time == 0x00FF))
      ident.security_enhanced_erase_time += 90;

    sscanf((char*)&tmpbuf[10],"%20s",ident.serial_number);
    sscanf((char*)&tmpbuf[23],"%8s",ident.firmware_revision);
    sscanf((char*)&tmpbuf[27],"%40c",ident.model_number);
  }

  int
  identify(const int     fd,
           sg::identity &ident)
  {
    int rv;
    int timeout;
    char buf[512];
    int buflen = sizeof(buf);
    struct ata_tf tf;

    tf_init(&tf,ATA_OP_IDENTIFY,0,1);

    timeout = 60000;
    rv = exec(fd,SG_READ,SG_PIO,&tf,buf,buflen,timeout);
    if(rv == 0)
      buf_to_identity(buf,ident);

    return rv;
  }

  int
  read_block(const int       fd_,
             const uint64_t  lba_,
             const uint64_t  blocks_,
             void           *buf_,
             const size_t    buflen_,
             const int       timeout_)
  {
    int blocks;
    int instruction;
    struct ata_tf tf;

    blocks = blocks_;
    if(blocks >= 65536)
      blocks = 0;
    instruction = ATA_OP_READ_PIO_EXT;
    tf_init(&tf,instruction,lba_,blocks);

    return exec(fd_,SG_READ,SG_PIO,&tf,buf_,buflen_,timeout_);
  }

  int
  write_block(const int       fd_,
              const uint64_t  lba_,
              const uint64_t  blocks_,
              const void     *buf_,
              const size_t    buflen_,
              const int       timeout_)
  {
    int blocks;
    int instruction;
    struct ata_tf tf;

    blocks = blocks_;
    if(blocks >= 65536)
      blocks = 0;
    instruction = ATA_OP_WRITE_PIO_EXT;
    tf_init(&tf,instruction,lba_,blocks);

    return exec(fd_,SG_WRITE,SG_PIO,&tf,(void*)buf_,buflen_,timeout_);
  }

  int
  flush_write_cache(const int fd,
                    const int timeout)
  {
    int rv;
    struct ata_tf tf;

    tf_init(&tf,ATA_OP_FLUSHCACHE_EXT,0,0);

    rv = exec(fd,SG_READ,SG_PIO,&tf,NULL,0,timeout);
    if(rv < 0)
      {
        tf_init(&tf,ATA_OP_FLUSHCACHE,0,0);

        rv = exec(fd,SG_READ,SG_PIO,&tf,NULL,0,timeout);
      }

    return rv;
  }

  int
  write_uncorrectable(const int      fd_,
                      const uint64_t lba_,
                      const uint8_t  type_,
                      const int      timeout_)
  {
    int rv;
    struct ata_tf tf;

    tf_init(&tf,ATA_OP_WRITE_UNC_EXT,lba_,1);
    tf.lob.feat = type_;
    tf.is_lba48 = 1;

    rv = exec(fd_,SG_READ,SG_PIO,&tf,NULL,0,timeout_);
    if(rv < 0)
      {
        char buf[520];

        tf_init(&tf,ATA_OP_WRITE_LONG_ONCE,lba_,1);
        memset(buf,0xA5,sizeof(buf));
        rv = exec(fd_,SG_WRITE,SG_PIO,&tf,buf,sizeof(buf),timeout_);
      }

    return rv;
  }

  int
  write_flagged_uncorrectable(const int      fd_,
                              const uint64_t lba_,
                              const bool     log_,
                              const int      timeout_)
  {
    uint8_t instr;

    instr = (log_ ?
             sg::ATA_WRITE_UNCORRECTABLE_EXT_FLAGGED_W_LOGGING :
             sg::ATA_WRITE_UNCORRECTABLE_EXT_FLAGGED_WO_LOGGING);

    return write_uncorrectable(fd_,lba_,instr,timeout_);
  }

  int
  write_pseudo_uncorrectable(const int      fd_,
                             const uint64_t lba_,
                             const bool     log_,
                             const int      timeout_)
  {
    uint8_t instr;

    instr = (log_ ?
             sg::ATA_WRITE_UNCORRECTABLE_EXT_PSEUDO_W_LOGGING :
             sg::ATA_WRITE_UNCORRECTABLE_EXT_PSEUDO_WO_LOGGING);

    return write_uncorrectable(fd_,lba_,instr,timeout_);
  }

  int
  security_set_password(const int  fd_,
                        const int  identifier_,
                        const char password_[32],
                        const int  timeout_)
  {
    uint8_t data[512];
    struct ata_tf tf;

    ::memset(data,0,512);
    ::memcpy(&data[2],password_,32);
    switch(identifier_)
      {
      case SG_IDENTIFIER_USER:
        data[0] = 0x00;
        break;
      case SG_IDENTIFIER_MASTER:
        data[0] = 0x01;
      default:
        return -EINVAL;
      }

    tf_init(&tf,ATA_OP_SECURITY_SET_PASSWORD,0,0);

    return exec(fd_,SG_WRITE,SG_PIO,&tf,data,512,timeout_);
  }

  int
  security_erase_prepare(const int fd_,
                         const int timeout_)
  {
    struct ata_tf tf;

    tf_init(&tf,ATA_OP_SECURITY_ERASE_PREPARE,0,0);

    return exec(fd_,SG_READ,SG_PIO,&tf,NULL,0,timeout_);
  }

  int
  security_erase(const int  fd_,
                 const int  erase_mode_,
                 const int  identifier_,
                 const char password_[32],
                 const int  timeout_)
  {
    struct ata_tf tf;
    uint8_t data[512];

    ::memset(data,0,512);
    ::memcpy(&data[2],password_,32);
    switch(identifier_)
      {
      case SG_IDENTIFIER_USER:
        data[0] |= 0x00;
        break;
      case SG_IDENTIFIER_MASTER:
        data[0] |= 0x01;
        break;
      default:
        return -EINVAL;
      }

    switch(erase_mode_)
      {
      case SG_ERASE_NORMAL:
        data[0] |= 0x00;
        break;
      case SG_ERASE_ENHANCED:
        data[0] |= 0x02;
        break;
      default:
        return -EINVAL;
      }

    tf_init(&tf,ATA_OP_SECURITY_ERASE_UNIT,0,1);

    return exec(fd_,SG_WRITE,SG_PIO,&tf,data,512,timeout_);
  }
}
