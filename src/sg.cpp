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
#include <linux/hdreg.h>
#include <scsi/sg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "sensedata.hpp"
#include "sg.hpp"

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

    if(io_hdr.status && (io_hdr.status != SG_CHECK_CONDITION))
      return -EBADE;
    if(io_hdr.host_status)
      return -EBADE;
    if(io_hdr.driver_status && (io_hdr.driver_status != SG_DRIVER_SENSE))
      return -EBADE;
    if(io_hdr.sbp[8] == 0x09) // ATA Return
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
    ident.smart_supported = (((buf16[83] >> 14) == 0x01) ? (buf16[82] & 0x0001) : 0);
    ident.smart_enabled   = (((buf16[87] >> 14) == 0x01) ? (buf16[85] & 0x0001) : 0);
    ident.write_uncorrectable_ext = (((buf16[83]  & 0xC000) == 0x4000) &&
                                     ((buf16[86]  & 0x8000) == 0x8000) &&
                                     ((buf16[119] & 0xC004) == 0x4004) &&
                                     ((buf16[120] & 0xC000) == 0x4000));

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
  read_block(const int       fd,
             const uint64_t  lba,
             const uint64_t  logical_block_size,
             void           *buf,
             const size_t    buflen,
             const int       timeout)
  {
    int           instruction;
    struct ata_tf tf;
    const int     nsect = (next_power_of_2(buflen) / logical_block_size);

    instruction = ((lba > LBA28_LIMIT) ? ATA_OP_READ_PIO_EXT : ATA_OP_READ_PIO);
    tf_init(&tf,instruction,lba,nsect);

    return exec(fd,SG_READ,SG_PIO,&tf,buf,buflen,timeout);
  }

  int
  write_block(const int       fd,
              const uint64_t  lba,
              const uint64_t  logical_block_size,
              const void     *buf,
              const size_t    buflen,
              const int       timeout)
  {
    int           instruction;
    struct ata_tf tf;
    const int     nsect = (next_power_of_2(buflen) / logical_block_size);

    instruction = ((lba > LBA28_LIMIT) ? ATA_OP_WRITE_PIO_EXT : ATA_OP_WRITE_PIO);
    tf_init(&tf,instruction,lba,nsect);

    return exec(fd,SG_WRITE,SG_PIO,&tf,(void*)buf,buflen,timeout);
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
  write_uncorrectable(const int      fd,
                      const uint64_t lba,
                      const uint8_t  type,
                      const int      timeout)
  {
    int rv;
    struct ata_tf tf;

    tf_init(&tf,ATA_OP_WRITE_UNC_EXT,lba,1);
    tf.lob.feat = type;
    tf.is_lba48 = 1;

    rv = exec(fd,SG_READ,SG_PIO,&tf,NULL,0,timeout);
    if(rv < 0)
      {
        char buf[520];

        tf_init(&tf,ATA_OP_WRITE_LONG_ONCE,lba,1);
        memset(buf,0xA5,sizeof(buf));
        rv = exec(fd,SG_WRITE,SG_PIO,&tf,buf,sizeof(buf),timeout);
      }

    return rv;
  }

  int
  write_flagged_uncorrectable(const int      fd,
                              const uint64_t lba,
                              const int      timeout)
  {
    return write_uncorrectable(fd,lba,sg::ATA_WRITE_UNCORRECTABLE_EXT_FLAGGED,timeout);
  }

  int
  write_pseudo_uncorrectable(const int      fd,
                             const uint64_t lba,
                             const int      timeout)
  {
    return write_uncorrectable(fd,lba,sg::ATA_WRITE_UNCORRECTABLE_EXT_PSEUDO,timeout);
  }
}
