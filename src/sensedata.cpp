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
#include <stdint.h>

namespace SenseData
{
  namespace Offset
  {
    enum Offset
      {
        RESPONSE_CODE                   = 0,
        SENSE_KEY                       = 1,
        ADDITIONAL_SENSE_CODE           = 2,
        ADDITIONAL_SENSE_CODE_QUALIFIER = 3,
        ADDITIONAL_SENSE_LENGTH         = 7
      };
  }

  namespace ResponseCode
  {
    enum ResponseCode
      {
        FIXED_CURRENT       = 0x70,
        FIXED_DEFERRED      = 0x71,
        DESCRIPTOR_CURRENT  = 0x72,
        DESCRIPTOR_DEFERRED = 0x73,
        VENDOR_SPECIFIC     = 0x7F
      };
  }

  namespace SenseKey
  {
    enum SenseKey
      {
        NO_SENSE        = 0x00,
        RECOVERED_ERROR = 0x01,
        NOT_READY       = 0x02,
        MEDIUM_ERROR    = 0x03,
        HARDWARE_ERROR  = 0x04,
        ILLEGAL_REQUEST = 0x05,
        UNIT_ATTENTION  = 0x06,
        DATA_PROTECT    = 0x07,
        BLANK_CHECK     = 0x08,
        VENDOR_SPECIFIC = 0x09,
        COPY_ABORTED    = 0x0A,
        ABORTED_COMMAND = 0x0B,
        OBSOLETE        = 0x0C,
        VOLUME_OVERFLOW = 0x0D,
        MISCOMPARE      = 0x0E,
        COMPLETED       = 0x0F
      };
  }

  uint8_t
  response_code(const uint8_t * const sb)
  {
    return (sb[Offset::RESPONSE_CODE] & 0x80);
  }

  uint8_t
  sense_key(const uint8_t * const sb)
  {
    return (sb[Offset::SENSE_KEY] & 0x0F);
  }

  uint8_t
  additional_sense_code(const uint8_t * const sb)
  {
    return sb[Offset::ADDITIONAL_SENSE_CODE];
  }

  uint8_t
  additional_sense_code_qualifier(const uint8_t * const sb)
  {
    return sb[Offset::ADDITIONAL_SENSE_CODE_QUALIFIER];
  }

  uint8_t
  additional_sense_length(const uint8_t * const sb)
  {
    return sb[Offset::ADDITIONAL_SENSE_LENGTH];
  }

  const char *
  error_to_string(const int error)
  {
    switch(error)
      {
      case 0x00000000:
        return "0000 No Additional Sense Information";
      case 0x00010000:
        return "0001 Filemark Detected";
      case 0x00020000:
        return "0002 End-Of-Partition/Medium Detected";
      case 0x00030000:
        return "0003 Setmark Detected";
      case 0x00040000:
        return "0004 Beginning-Of-Partition/Medium Detected";
      case 0x00050000:
        return "0005 End-Of-Data Detected";
      case 0x00060000:
        return "0006 I/O Process Terminated";
      case 0x00070000:
        return "0007 Programmable Early Warning Detected";
      case 0x00110000:
        return "0011 Audio Play Operation In Progress";
      case 0x00120000:
        return "0012 Audio Play Operation Paused";
      case 0x00130000:
        return "0013 Audio Play Operation Successfully Completed";
      case 0x00140000:
        return "0014 Audio Play Operation Stopped Due To Error";
      case 0x00150000:
        return "0015 No Current Audio Status To Return";
      case 0x00160000:
        return "0016 Operation In Progress";
      case 0x00170000:
        return "0017 Cleaning Requested";
      case 0x00180000:
        return "0018 Erase Operation In Progress";
      case 0x00190000:
        return "0019 Locate Operation In Progress";
      case 0x001A0000:
        return "001a Rewind Operation In Progress";
      case 0x001B0000:
        return "001b Set Capacity Operation In Progress";
      case 0x001C0000:
        return "001c Verify Operation In Progress";
      case 0x001D0000:
        return "001d Ata Pass Through Information Available";
      case 0x001E0000:
        return "001e Conflicting Sa Creation Request";
      case 0x001F0000:
        return "001f Logical Unit Transitioning To Another Power Condition";
      case 0x00200000:
        return "0020 Extended Copy Information Available";
      case 0x00210000:
        return "0021 Atomic Command Aborted Due To Aca";
      case 0x01000000:
        return "0100 No Index/Sector Signal";
      case 0x02000000:
        return "0200 No Seek Complete";
      case 0x03000000:
        return "0300 Peripheral Device Write Fault";
      case 0x03010000:
        return "0301 No Write Current";
      case 0x03020000:
        return "0302 Excessive Write Errors";
      case 0x04000000:
        return "0400 Logical Unit Not Ready, Cause Not Reportable";
      case 0x04010000:
        return "0401 Logical Unit Is In Process Of Becoming Ready";
      case 0x04020000:
        return "0402 Logical Unit Not Ready, Initializing Command Required";
      case 0x04030000:
        return "0403 Logical Unit Not Ready, Manual Intervention Required";
      case 0x04040000:
        return "0404 Logical Unit Not Ready, Format In Progress";
      case 0x04050000:
        return "0405 Logical Unit Not Ready, Rebuild In Progress";
      case 0x04060000:
        return "0406 Logical Unit Not Ready, Recalculation In Progress";
      case 0x04070000:
        return "0407 Logical Unit Not Ready, Operation In Progress";
      case 0x04080000:
        return "0408 Logical Unit Not Ready, Long Write In Progress";
      case 0x04090000:
        return "0409 Logical Unit Not Ready, Self-Test In Progress";
      case 0x040A0000:
        return "040a Logical Unit Not Accessible, Asymmetric Access State Transition";
      case 0x040B0000:
        return "040b Logical Unit Not Accessible, Target Port In Standby State";
      case 0x040C0000:
        return "040c Logical Unit Not Accessible, Target Port In Unavailable State";
      case 0x040D0000:
        return "040d Logical Unit Not Ready, Structure Check Required";
      case 0x040E0000:
        return "040e Logical Unit Not Ready, Security Session In Progress";
      case 0x04100000:
        return "0410 Logical Unit Not Ready, Auxiliary Memory Not Accessible";
      case 0x04110000:
        return "0411 Logical Unit Not Ready, Notify (Enable Spinup) Required";
      case 0x04120000:
        return "0412 Logical Unit Not Ready, Offline";
      case 0x04130000:
        return "0413 Logical Unit Not Ready, Sa Creation In Progress";
      case 0x04140000:
        return "0414 Logical Unit Not Ready, Space Allocation In Progress";
      case 0x04150000:
        return "0415 Logical Unit Not Ready, Robotics Disabled";
      case 0x04160000:
        return "0416 Logical Unit Not Ready, Configuration Required";
      case 0x04170000:
        return "0417 Logical Unit Not Ready, Calibration Required";
      case 0x04180000:
        return "0418 Logical Unit Not Ready, A Door Is Open";
      case 0x04190000:
        return "0419 Logical Unit Not Ready, Operating In Sequential Mode";
      case 0x041A0000:
        return "041a Logical Unit Not Ready, Start Stop Unit Command In Progress";
      case 0x041B0000:
        return "041b Logical Unit Not Ready, Sanitize In Progress";
      case 0x041C0000:
        return "041c Logical Unit Not Ready, Additional Power Use Not Yet Granted";
      case 0x041D0000:
        return "041d Logical Unit Not Ready, Configuration In Progress";
      case 0x041E0000:
        return "041e Logical Unit Not Ready, Microcode Activation Required";
      case 0x041F0000:
        return "041f Logical Unit Not Ready, Microcode Download Required";
      case 0x04200000:
        return "0420 Logical Unit Not Ready, Logical Unit Reset Required";
      case 0x04210000:
        return "0421 Logical Unit Not Ready, Hard Reset Required";
      case 0x04220000:
        return "0422 Logical Unit Not Ready, Power Cycle Required";
      case 0x04230000:
        return "0423 Logical Unit Not Ready, Affiliation Required";
      case 0x05000000:
        return "0500 Logical Unit Does Not Respond To Selection";
      case 0x06000000:
        return "0600 No Reference Position Found";
      case 0x07000000:
        return "0700 Multiple Peripheral Devices Selected";
      case 0x08000000:
        return "0800 Logical Unit Communication Failure";
      case 0x08010000:
        return "0801 Logical Unit Communication Time-Out";
      case 0x08020000:
        return "0802 Logical Unit Communication Parity Error";
      case 0x08030000:
        return "0803 Logical Unit Communication Crc Error (Ultra-Dma/32)";
      case 0x08040000:
        return "0804 Unreachable Copy Target";
      case 0x09000000:
        return "0900 Track Following Error";
      case 0x09010000:
        return "0901 Tracking Servo Failure";
      case 0x09020000:
        return "0902 Focus Servo Failure";
      case 0x09030000:
        return "0903 Spindle Servo Failure";
      case 0x09040000:
        return "0904 Head Select Fault";
      case 0x09050000:
        return "0905 Vibration Induced Tracking Error";
      case 0x0A000000:
        return "0a00 Error Log Overflow";
      case 0x0B000000:
        return "0b00 Warning";
      case 0x0B010000:
        return "0b01 Warning - Specified Temperature Exceeded";
      case 0x0B020000:
        return "0b02 Warning - Enclosure Degraded";
      case 0x0B030000:
        return "0b03 Warning - Background Self-Test Failed";
      case 0x0B040000:
        return "0b04 Warning - Background Pre-Scan Detected Medium Error";
      case 0x0B050000:
        return "0b05 Warning - Background Medium Scan Detected Medium Error";
      case 0x0B060000:
        return "0b06 Warning - Non-Volatile Cache Now Volatile";
      case 0x0B070000:
        return "0b07 Warning - Degraded Power To Non-Volatile Cache";
      case 0x0B080000:
        return "0b08 Warning - Power Loss Expected";
      case 0x0B090000:
        return "0b09 Warning - Device Statistics Notification Active";
      case 0x0B0A0000:
        return "0b0a Warning - High Critical Temperature Limit Exceeded";
      case 0x0B0B0000:
        return "0b0b Warning - Low Critical Temperature Limit Exceeded";
      case 0x0B0C0000:
        return "0b0c Warning - High Operating Temperature Limit Exceeded";
      case 0x0B0D0000:
        return "0b0d Warning - Low Operating Temperature Limit Exceeded";
      case 0x0B0E0000:
        return "0b0e Warning - High Critical Humidity Limit Exceeded";
      case 0x0B0F0000:
        return "0b0f Warning - Low Critical Humidity Limit Exceeded";
      case 0x0B100000:
        return "0b10 Warning - High Operating Humidity Limit Exceeded";
      case 0x0B110000:
        return "0b11 Warning - Low Operating Humidity Limit Exceeded";
      case 0x0C000000:
        return "0c00 Write Error";
      case 0x0C010000:
        return "0c01 Write Error - Recovered With Auto Reallocation";
      case 0x0C020000:
        return "0c02 Write Error - Auto Reallocation Failed";
      case 0x0C030000:
        return "0c03 Write Error - Recommend Reassignment";
      case 0x0C040000:
        return "0c04 Compression Check Miscompare Error";
      case 0x0C050000:
        return "0c05 Data Expansion Occurred During Compression";
      case 0x0C060000:
        return "0c06 Block Not Compressible";
      case 0x0C070000:
        return "0c07 Write Error - Recovery Needed";
      case 0x0C080000:
        return "0c08 Write Error - Recovery Failed";
      case 0x0C090000:
        return "0c09 Write Error - Loss Of Streaming";
      case 0x0C0A0000:
        return "0c0a Write Error - Padding Blocks Added";
      case 0x0C0B0000:
        return "0c0b Auxiliary Memory Write Error";
      case 0x0C0C0000:
        return "0c0c Write Error - Unexpected Unsolicited Data";
      case 0x0C0D0000:
        return "0c0d Write Error - Not Enough Unsolicited Data";
      case 0x0C0E0000:
        return "0c0e Multiple Write Errors";
      case 0x0C0F0000:
        return "0c0f Defects In Error Window";
      case 0x0C100000:
        return "0c10 Incomplete Multiple Atomic Write Operations";
      case 0x0C110000:
        return "0c11 Write Error - Recovery Scan Needed";
      case 0x0C120000:
        return "0c12 Write Error - Insufficient Zone Resources";
      case 0x0D000000:
        return "0d00 Error Detected By Third Party Temporary Initiator";
      case 0x0D010000:
        return "0d01 Third Party Device Failure";
      case 0x0D020000:
        return "0d02 Copy Target Device Not Reachable";
      case 0x0D030000:
        return "0d03 Incorrect Copy Target Device Type";
      case 0x0D040000:
        return "0d04 Copy Target Device Data Underrun";
      case 0x0D050000:
        return "0d05 Copy Target Device Data Overrun";
      case 0x0E000000:
        return "0e00 Invalid Information Unit";
      case 0x0E010000:
        return "0e01 Information Unit Too Short";
      case 0x0E020000:
        return "0e02 Information Unit Too Long";
      case 0x0E030000:
        return "0e03 Invalid Field In Command Information Unit";
      case 0x0F000000:
        return "0f00";
      case 0x10000000:
        return "1000 Id Crc Or Ecc Error";
      case 0x10010000:
        return "1001 Logical Block Guard Check Failed";
      case 0x10020000:
        return "1002 Logical Block Application Tag Check Failed";
      case 0x10030000:
        return "1003 Logical Block Reference Tag Check Failed";
      case 0x10040000:
        return "1004 Logical Block Protection Error On Recover Buffered Data";
      case 0x10050000:
        return "1005 Logical Block Protection Method Error";
      case 0x11000000:
        return "1100 Unrecovered Read Error";
      case 0x11010000:
        return "1101 Read Retries Exhausted";
      case 0x11020000:
        return "1102 Error Too Long To Correct";
      case 0x11030000:
        return "1103 Multiple Read Errors";
      case 0x11040000:
        return "1104 Unrecovered Read Error - Auto Reallocate Failed";
      case 0x11050000:
        return "1105 L-Ec Uncorrectable Error";
      case 0x11060000:
        return "1106 Circ Unrecovered Error";
      case 0x11070000:
        return "1107 Data Re-Synchronization Error";
      case 0x11080000:
        return "1108 Incomplete Block Read";
      case 0x11090000:
        return "1109 No Gap Found";
      case 0x110A0000:
        return "110a Miscorrected Error";
      case 0x110B0000:
        return "110b Unrecovered Read Error - Recommend Reassignment";
      case 0x110C0000:
        return "110c Unrecovered Read Error - Recommend Rewrite The Data";
      case 0x110D0000:
        return "110d De-Compression Crc Error";
      case 0x110E0000:
        return "110e Cannot Decompress Using Declared Algorithm";
      case 0x110F0000:
        return "110f Error Reading Upc/Ean Number";
      case 0x11100000:
        return "1110 Error Reading Isrc Number";
      case 0x11110000:
        return "1111 Read Error - Loss Of Streaming";
      case 0x11120000:
        return "1112 Auxiliary Memory Read Error";
      case 0x11130000:
        return "1113 Read Error - Failed Retransmission Request";
      case 0x11140000:
        return "1114 Read Error - Lba Marked Bad By Application Client";
      case 0x11150000:
        return "1115 Write After Sanitize Required";
      case 0x12000000:
        return "1200 Address Mark Not Found For Id Field";
      case 0x13000000:
        return "1300 Address Mark Not Found For Data Field";
      case 0x14000000:
        return "1400 Recorded Entity Not Found";
      case 0x14010000:
        return "1401 Record Not Found";
      case 0x14020000:
        return "1402 Filemark Or Setmark Not Found";
      case 0x14030000:
        return "1403 End-Of-Data Not Found";
      case 0x14040000:
        return "1404 Block Sequence Error";
      case 0x14050000:
        return "1405 Record Not Found - Recommend Reassignment";
      case 0x14060000:
        return "1406 Record Not Found - Data Auto-Reallocated";
      case 0x14070000:
        return "1407 Locate Operation Failure";
      case 0x15000000:
        return "1500 Random Positioning Error";
      case 0x15010000:
        return "1501 Mechanical Positioning Error";
      case 0x15020000:
        return "1502 Positioning Error Detected By Read Of Medium";
      case 0x16000000:
        return "1600 Data Synchronization Mark Error";
      case 0x16010000:
        return "1601 Data Sync Error - Data Rewritten";
      case 0x16020000:
        return "1602 Data Sync Error - Recommend Rewrite";
      case 0x16030000:
        return "1603 Data Sync Error - Data Auto-Reallocated";
      case 0x16040000:
        return "1604 Data Sync Error - Recommend Reassignment";
      case 0x17000000:
        return "1700 Recovered Data With No Error Correction Applied";
      case 0x17010000:
        return "1701 Recovered Data With Retries";
      case 0x17020000:
        return "1702 Recovered Data With Positive Head Offset";
      case 0x17030000:
        return "1703 Recovered Data With Negative Head Offset";
      case 0x17040000:
        return "1704 Recovered Data With Retries And/Or Circ Applied";
      case 0x17050000:
        return "1705 Recovered Data Using Previous Sector Id";
      case 0x17060000:
        return "1706 Recovered Data Without Ecc - Data Auto-Reallocated";
      case 0x17070000:
        return "1707 Recovered Data Without Ecc - Recommend Reassignment";
      case 0x17080000:
        return "1708 Recovered Data Without Ecc - Recommend Rewrite";
      case 0x17090000:
        return "1709 Recovered Data Without Ecc - Data Rewritten";
      case 0x18000000:
        return "1800 Recovered Data With Error Correction Applied";
      case 0x18010000:
        return "1801 Recovered Data With Error Corr. & Retries Applied";
      case 0x18020000:
        return "1802 Recovered Data - Data Auto-Reallocated";
      case 0x18030000:
        return "1803 Recovered Data With Circ";
      case 0x18040000:
        return "1804 Recovered Data With L-Ec";
      case 0x18050000:
        return "1805 Recovered Data - Recommend Reassignment";
      case 0x18060000:
        return "1806 Recovered Data - Recommend Rewrite";
      case 0x18070000:
        return "1807 Recovered Data With Ecc - Data Rewritten";
      case 0x18080000:
        return "1808 Recovered Data With Linking";
      case 0x19000000:
        return "1900 Defect List Error";
      case 0x19010000:
        return "1901 Defect List Not Available";
      case 0x19020000:
        return "1902 Defect List Error In Primary List";
      case 0x19030000:
        return "1903 Defect List Error In Grown List";
      case 0x1A000000:
        return "1a00 Parameter List Length Error";
      case 0x1B000000:
        return "1b00 Synchronous Data Transfer Error";
      case 0x1C000000:
        return "1c00 Defect List Not Found";
      case 0x1C010000:
        return "1c01 Primary Defect List Not Found";
      case 0x1C020000:
        return "1c02 Grown Defect List Not Found";
      case 0x1D000000:
        return "1d00 Miscompare During Verify Operation";
      case 0x1D010000:
        return "1d01 Miscompare Verify Of Unmapped Lba";
      case 0x1E000000:
        return "1e00 Recovered Id With Ecc Correction";
      case 0x1F000000:
        return "1f00 Partial Defect List Transfer";
      case 0x20000000:
        return "2000 Invalid Command Operation Code";
      case 0x20010000:
        return "2001 Access Denied - Initiator Pending-Enrolled";
      case 0x20020000:
        return "2002 Access Denied - No Access Rights";
      case 0x20030000:
        return "2003 Access Denied - Invalid Mgmt Id Key";
      case 0x20040000:
        return "2004 Illegal Command While In Write Capable State";
      case 0x20050000:
        return "2005 Obsolete";
      case 0x20060000:
        return "2006 Illegal Command While In Explicit Address Mode";
      case 0x20070000:
        return "2007 Illegal Command While In Implicit Address Mode";
      case 0x20080000:
        return "2008 Access Denied - Enrollment Conflict";
      case 0x20090000:
        return "2009 Access Denied - Invalid Lu Identifier";
      case 0x200A0000:
        return "200a Access Denied - Invalid Proxy Token";
      case 0x200B0000:
        return "200b Access Denied - Acl Lun Conflict";
      case 0x200C0000:
        return "200c Illegal Command When Not In Append-Only Mode";
      case 0x200D0000:
        return "200d Not An Administrative Logical Unit";
      case 0x200E0000:
        return "200e Not A Subsidiary Logical Unit";
      case 0x200F0000:
        return "200f Not A Conglomerate Logical Unit";
      case 0x21000000:
        return "2100 Logical Block Address Out Of Range";
      case 0x21010000:
        return "2101 Invalid Element Address";
      case 0x21020000:
        return "2102 Invalid Address For Write";
      case 0x21030000:
        return "2103 Invalid Write Crossing Layer Jump";
      case 0x21040000:
        return "2104 Unaligned Write Command";
      case 0x21050000:
        return "2105 Write Boundary Violation";
      case 0x21060000:
        return "2106 Attempt To Read Invalid Data";
      case 0x21070000:
        return "2107 Read Boundary Violation";
      case 0x21080000:
        return "2108 Misaligned Write Command";
      case 0x22000000:
        return "2200 Illegal Function (Use 20 00, 24 00, Or 26 00)";
      case 0x23000000:
        return "2300 Invalid Token Operation, Cause Not Reportable";
      case 0x23010000:
        return "2301 Invalid Token Operation, Unsupported Token Type";
      case 0x23020000:
        return "2302 Invalid Token Operation, Remote Token Usage Not Supported";
      case 0x23030000:
        return "2303 Invalid Token Operation, Remote Rod Token Creation Not Supported";
      case 0x23040000:
        return "2304 Invalid Token Operation, Token Unknown";
      case 0x23050000:
        return "2305 Invalid Token Operation, Token Corrupt";
      case 0x23060000:
        return "2306 Invalid Token Operation, Token Revoked";
      case 0x23070000:
        return "2307 Invalid Token Operation, Token Expired";
      case 0x23080000:
        return "2308 Invalid Token Operation, Token Cancelled";
      case 0x23090000:
        return "2309 Invalid Token Operation, Token Deleted";
      case 0x230A0000:
        return "230a Invalid Token Operation, Invalid Token Length";
      case 0x24000000:
        return "2400 Invalid Field In Cdb";
      case 0x24010000:
        return "2401 Cdb Decryption Error";
      case 0x24020000:
        return "2402 Obsolete";
      case 0x24030000:
        return "2403 Obsolete";
      case 0x24040000:
        return "2404 Security Audit Value Frozen";
      case 0x24050000:
        return "2405 Security Working Key Frozen";
      case 0x24060000:
        return "2406 Nonce Not Unique";
      case 0x24070000:
        return "2407 Nonce Timestamp Out Of Range";
      case 0x24080000:
        return "2408 Invalid Xcdb";
      case 0x24090000:
        return "2409 Invalid Fast Format";
      case 0x25000000:
        return "2500 Logical Unit Not Supported";
      case 0x26000000:
        return "2600 Invalid Field In Parameter List";
      case 0x26010000:
        return "2601 Parameter Not Supported";
      case 0x26020000:
        return "2602 Parameter Value Invalid";
      case 0x26030000:
        return "2603 Threshold Parameters Not Supported";
      case 0x26040000:
        return "2604 Invalid Release Of Persistent Reservation";
      case 0x26050000:
        return "2605 Data Decryption Error";
      case 0x26060000:
        return "2606 Too Many Target Descriptors";
      case 0x26070000:
        return "2607 Unsupported Target Descriptor Type Code";
      case 0x26080000:
        return "2608 Too Many Segment Descriptors";
      case 0x26090000:
        return "2609 Unsupported Segment Descriptor Type Code";
      case 0x260A0000:
        return "260a Unexpected Inexact Segment";
      case 0x260B0000:
        return "260b Inline Data Length Exceeded";
      case 0x260C0000:
        return "260c Invalid Operation For Copy Source Or Destination";
      case 0x260D0000:
        return "260d Copy Segment Granularity Violation";
      case 0x260E0000:
        return "260e Invalid Parameter While Port Is Enabled";
      case 0x260F0000:
        return "260f Invalid Data-Out Buffer Integrity Check Value";
      case 0x26100000:
        return "2610 Data Decryption Key Fail Limit Reached";
      case 0x26110000:
        return "2611 Incomplete Key-Associated Data Set";
      case 0x26120000:
        return "2612 Vendor Specific Key Reference Not Found";
      case 0x26130000:
        return "2613 Application Tag Mode Page Is Invalid";
      case 0x26140000:
        return "2614 Tape Stream Mirroring Prevented";
      case 0x26150000:
        return "2615 Copy Source Or Copy Destination Not Authorized";
      case 0x27000000:
        return "2700 Write Protected";
      case 0x27010000:
        return "2701 Hardware Write Protected";
      case 0x27020000:
        return "2702 Logical Unit Software Write Protected";
      case 0x27030000:
        return "2703 Associated Write Protect";
      case 0x27040000:
        return "2704 Persistent Write Protect";
      case 0x27050000:
        return "2705 Permanent Write Protect";
      case 0x27060000:
        return "2706 Conditional Write Protect";
      case 0x27070000:
        return "2707 Space Allocation Failed Write Protect";
      case 0x27080000:
        return "2708 Zone Is Read Only";
      case 0x28000000:
        return "2800 Not Ready To Ready Change, Medium May Have Changed";
      case 0x28010000:
        return "2801 Import Or Export Element Accessed";
      case 0x28020000:
        return "2802 Format-Layer May Have Changed";
      case 0x28030000:
        return "2803 Import/Export Element Accessed, Medium Changed";
      case 0x29000000:
        return "2900 Power On, Reset, Or Bus Device Reset Occurred";
      case 0x29010000:
        return "2901 Power On Occurred";
      case 0x29020000:
        return "2902 Scsi Bus Reset Occurred";
      case 0x29030000:
        return "2903 Bus Device Reset Function Occurred";
      case 0x29040000:
        return "2904 Device Internal Reset";
      case 0x29050000:
        return "2905 Transceiver Mode Changed To Single-Ended";
      case 0x29060000:
        return "2906 Transceiver Mode Changed To Lvd";
      case 0x29070000:
        return "2907 I_t Nexus Loss Occurred";
      case 0x2A000000:
        return "2a00 Parameters Changed";
      case 0x2A010000:
        return "2a01 Mode Parameters Changed";
      case 0x2A020000:
        return "2a02 Log Parameters Changed";
      case 0x2A030000:
        return "2a03 Reservations Preempted";
      case 0x2A040000:
        return "2a04 Reservations Released";
      case 0x2A050000:
        return "2a05 Registrations Preempted";
      case 0x2A060000:
        return "2a06 Asymmetric Access State Changed";
      case 0x2A070000:
        return "2a07 Implicit Asymmetric Access State Transition Failed";
      case 0x2A080000:
        return "2a08 Priority Changed";
      case 0x2A090000:
        return "2a09 Capacity Data Has Changed";
      case 0x2A0A0000:
        return "2a0a Error History I_t Nexus Cleared";
      case 0x2A0B0000:
        return "2a0b Error History Snapshot Released";
      case 0x2A0C0000:
        return "2a0c Error Recovery Attributes Have Changed";
      case 0x2A0D0000:
        return "2a0d Data Encryption Capabilities Changed";
      case 0x2A100000:
        return "2a10 Timestamp Changed";
      case 0x2A110000:
        return "2a11 Data Encryption Parameters Changed By Another I_t Nexus";
      case 0x2A120000:
        return "2a12 Data Encryption Parameters Changed By Vendor Specific Event";
      case 0x2A130000:
        return "2a13 Data Encryption Key Instance Counter Has Changed";
      case 0x2A140000:
        return "2a14 Sa Creation Capabilities Data Has Changed";
      case 0x2A150000:
        return "2a15 Medium Removal Prevention Preempted";
      case 0x2A160000:
        return "2a16 Zone Reset Write Pointer Recommended";
      case 0x2B000000:
        return "2b00 Copy Cannot Execute Since Host Cannot Disconnect";
      case 0x2C000000:
        return "2c00 Command Sequence Error";
      case 0x2C010000:
        return "2c01 Too Many Windows Specified";
      case 0x2C020000:
        return "2c02 Invalid Combination Of Windows Specified";
      case 0x2C030000:
        return "2c03 Current Program Area Is Not Empty";
      case 0x2C040000:
        return "2c04 Current Program Area Is Empty";
      case 0x2C050000:
        return "2c05 Illegal Power Condition Request";
      case 0x2C060000:
        return "2c06 Persistent Prevent Conflict";
      case 0x2C070000:
        return "2c07 Previous Busy Status";
      case 0x2C080000:
        return "2c08 Previous Task Set Full Status";
      case 0x2C090000:
        return "2c09 Previous Reservation Conflict Status";
      case 0x2C0A0000:
        return "2c0a Partition Or Collection Contains User Objects";
      case 0x2C0B0000:
        return "2c0b Not Reserved";
      case 0x2C0C0000:
        return "2c0c Orwrite Generation Does Not Match";
      case 0x2C0D0000:
        return "2c0d Reset Write Pointer Not Allowed";
      case 0x2C0E0000:
        return "2c0e Zone Is Offline";
      case 0x2C0F0000:
        return "2c0f Stream Not Open";
      case 0x2C100000:
        return "2c10 Unwritten Data In Zone";
      case 0x2C110000:
        return "2c11 Descriptor Format Sense Data Required";
      case 0x2D000000:
        return "2d00 Overwrite Error On Update In Place";
      case 0x2E000000:
        return "2e00 Insufficient Time For Operation";
      case 0x2E010000:
        return "2e01 Command Timeout Before Processing";
      case 0x2E020000:
        return "2e02 Command Timeout During Processing";
      case 0x2E030000:
        return "2e03 Command Timeout During Processing Due To Error Recovery";
      case 0x2F000000:
        return "2f00 Commands Cleared By Another Initiator";
      case 0x2F010000:
        return "2f01 Commands Cleared By Power Loss Notification";
      case 0x2F020000:
        return "2f02 Commands Cleared By Device Server";
      case 0x2F030000:
        return "2f03 Some Commands Cleared By Queuing Layer Event";
      case 0x30000000:
        return "3000 Incompatible Medium Installed";
      case 0x30010000:
        return "3001 Cannot Read Medium - Unknown Format";
      case 0x30020000:
        return "3002 Cannot Read Medium - Incompatible Format";
      case 0x30030000:
        return "3003 Cleaning Cartridge Installed";
      case 0x30040000:
        return "3004 Cannot Write Medium - Unknown Format";
      case 0x30050000:
        return "3005 Cannot Write Medium - Incompatible Format";
      case 0x30060000:
        return "3006 Cannot Format Medium - Incompatible Medium";
      case 0x30070000:
        return "3007 Cleaning Failure";
      case 0x30080000:
        return "3008 Cannot Write - Application Code Mismatch";
      case 0x30090000:
        return "3009 Current Session Not Fixated For Append";
      case 0x300A0000:
        return "300a Cleaning Request Rejected";
      case 0x300C0000:
        return "300c Worm Medium - Overwrite Attempted";
      case 0x300D0000:
        return "300d Worm Medium - Integrity Check";
      case 0x30100000:
        return "3010 Medium Not Formatted";
      case 0x30110000:
        return "3011 Incompatible Volume Type";
      case 0x30120000:
        return "3012 Incompatible Volume Qualifier";
      case 0x30130000:
        return "3013 Cleaning Volume Expired";
      case 0x31000000:
        return "3100 Medium Format Corrupted";
      case 0x31010000:
        return "3101 Format Command Failed";
      case 0x31020000:
        return "3102 Zoned Formatting Failed Due To Spare Linking";
      case 0x31030000:
        return "3103 Sanitize Command Failed";
      case 0x32000000:
        return "3200 No Defect Spare Location Available";
      case 0x32010000:
        return "3201 Defect List Update Failure";
      case 0x33000000:
        return "3300 Tape Length Error";
      case 0x34000000:
        return "3400 Enclosure Failure";
      case 0x35000000:
        return "3500 Enclosure Services Failure";
      case 0x35010000:
        return "3501 Unsupported Enclosure Function";
      case 0x35020000:
        return "3502 Enclosure Services Unavailable";
      case 0x35030000:
        return "3503 Enclosure Services Transfer Failure";
      case 0x35040000:
        return "3504 Enclosure Services Transfer Refused";
      case 0x35050000:
        return "3505 Enclosure Services Checksum Error";
      case 0x36000000:
        return "3600 Ribbon, Ink, Or Toner Failure";
      case 0x37000000:
        return "3700 Rounded Parameter";
      case 0x38000000:
        return "3800 Event Status Notification";
      case 0x38020000:
        return "3802 Esn - Power Management Class Event";
      case 0x38040000:
        return "3804 Esn - Media Class Event";
      case 0x38060000:
        return "3806 Esn - Device Busy Class Event";
      case 0x38070000:
        return "3807 Thin Provisioning Soft Threshold Reached";
      case 0x39000000:
        return "3900 Saving Parameters Not Supported";
      case 0x3A000000:
        return "3a00 Medium Not Present";
      case 0x3A010000:
        return "3a01 Medium Not Present - Tray Closed";
      case 0x3A020000:
        return "3a02 Medium Not Present - Tray Open";
      case 0x3A030000:
        return "3a03 Medium Not Present - Loadable";
      case 0x3A040000:
        return "3a04 Medium Not Present - Medium Auxiliary Memory Accessible";
      case 0x3B000000:
        return "3b00 Sequential Positioning Error";
      case 0x3B010000:
        return "3b01 Tape Position Error At Beginning-Of-Medium";
      case 0x3B020000:
        return "3b02 Tape Position Error At End-Of-Medium";
      case 0x3B030000:
        return "3b03 Tape Or Electronic Vertical Forms Unit Not Ready";
      case 0x3B040000:
        return "3b04 Slew Failure";
      case 0x3B050000:
        return "3b05 Paper Jam";
      case 0x3B060000:
        return "3b06 Failed To Sense Top-Of-Form";
      case 0x3B070000:
        return "3b07 Failed To Sense Bottom-Of-Form";
      case 0x3B080000:
        return "3b08 Reposition Error";
      case 0x3B090000:
        return "3b09 Read Past End Of Medium";
      case 0x3B0A0000:
        return "3b0a Read Past Beginning Of Medium";
      case 0x3B0B0000:
        return "3b0b Position Past End Of Medium";
      case 0x3B0C0000:
        return "3b0c Position Past Beginning Of Medium";
      case 0x3B0D0000:
        return "3b0d Medium Destination Element Full";
      case 0x3B0E0000:
        return "3b0e Medium Source Element Empty";
      case 0x3B0F0000:
        return "3b0f End Of Medium Reached";
      case 0x3B110000:
        return "3b11 Medium Magazine Not Accessible";
      case 0x3B120000:
        return "3b12 Medium Magazine Removed";
      case 0x3B130000:
        return "3b13 Medium Magazine Inserted";
      case 0x3B140000:
        return "3b14 Medium Magazine Locked";
      case 0x3B150000:
        return "3b15 Medium Magazine Unlocked";
      case 0x3B160000:
        return "3b16 Mechanical Positioning Or Changer Error";
      case 0x3B170000:
        return "3b17 Read Past End Of User Object";
      case 0x3B180000:
        return "3b18 Element Disabled";
      case 0x3B190000:
        return "3b19 Element Enabled";
      case 0x3B1A0000:
        return "3b1a Data Transfer Device Removed";
      case 0x3B1B0000:
        return "3b1b Data Transfer Device Inserted";
      case 0x3B1C0000:
        return "3b1c Too Many Logical Objects On Partition To Support Operation";
      case 0x3C000000:
        return "3c00";
      case 0x3D000000:
        return "3d00 Invalid Bits In Identify Message";
      case 0x3E000000:
        return "3e00 Logical Unit Has Not Self-Configured Yet";
      case 0x3E010000:
        return "3e01 Logical Unit Failure";
      case 0x3E020000:
        return "3e02 Timeout On Logical Unit";
      case 0x3E030000:
        return "3e03 Logical Unit Failed Self-Test";
      case 0x3E040000:
        return "3e04 Logical Unit Unable To Update Self-Test Log";
      case 0x3F000000:
        return "3f00 Target Operating Conditions Have Changed";
      case 0x3F010000:
        return "3f01 Microcode Has Been Changed";
      case 0x3F020000:
        return "3f02 Changed Operating Definition";
      case 0x3F030000:
        return "3f03 Inquiry Data Has Changed";
      case 0x3F040000:
        return "3f04 Component Device Attached";
      case 0x3F050000:
        return "3f05 Device Identifier Changed";
      case 0x3F060000:
        return "3f06 Redundancy Group Created Or Modified";
      case 0x3F070000:
        return "3f07 Redundancy Group Deleted";
      case 0x3F080000:
        return "3f08 Spare Created Or Modified";
      case 0x3F090000:
        return "3f09 Spare Deleted";
      case 0x3F0A0000:
        return "3f0a Volume Set Created Or Modified";
      case 0x3F0B0000:
        return "3f0b Volume Set Deleted";
      case 0x3F0C0000:
        return "3f0c Volume Set Deassigned";
      case 0x3F0D0000:
        return "3f0d Volume Set Reassigned";
      case 0x3F0E0000:
        return "3f0e Reported Luns Data Has Changed";
      case 0x3F0F0000:
        return "3f0f Echo Buffer Overwritten";
      case 0x3F100000:
        return "3f10 Medium Loadable";
      case 0x3F110000:
        return "3f11 Medium Auxiliary Memory Accessible";
      case 0x3F120000:
        return "3f12 Iscsi Ip Address Added";
      case 0x3F130000:
        return "3f13 Iscsi Ip Address Removed";
      case 0x3F140000:
        return "3f14 Iscsi Ip Address Changed";
      case 0x3F150000:
        return "3f15 Inspect Referrals Sense Descriptors";
      case 0x3F160000:
        return "3f16 Microcode Has Been Changed Without Reset";
      case 0x3F170000:
        return "3f17 Zone Transition To Full";
      case 0x3F180000:
        return "3f18 Bind Completed";
      case 0x3F190000:
        return "3f19 Bind Redirected";
      case 0x3F1A0000:
        return "3f1a Subsidiary Binding Changed";
      case 0x40000000:
        return "4000 Ram Failure (Should Use 40 Nn)";
        // case 0x40NN0000:
        //   return "40nn Diagnostic Failure On Component Nn (80h-Ffh)";
      case 0x41000000:
        return "4100 Data Path Failure (Should Use 40 Nn)";
      case 0x42000000:
        return "4200 Power-On Or Self-Test Failure (Should Use 40 Nn)";
      case 0x43000000:
        return "4300 Message Error";
      case 0x44000000:
        return "4400 Internal Target Failure";
      case 0x44010000:
        return "4401 Persistent Reservation Information Lost";
      case 0x44710000:
        return "4471 Ata Device Failed Set Features";
      case 0x45000000:
        return "4500 Select Or Reselect Failure";
      case 0x46000000:
        return "4600 Unsuccessful Soft Reset";
      case 0x47000000:
        return "4700 Scsi Parity Error";
      case 0x47010000:
        return "4701 Data Phase Crc Error Detected";
      case 0x47020000:
        return "4702 Scsi Parity Error Detected During St Data Phase";
      case 0x47030000:
        return "4703 Information Unit Iucrc Error Detected";
      case 0x47040000:
        return "4704 Asynchronous Information Protection Error Detected";
      case 0x47050000:
        return "4705 Protocol Service Crc Error";
      case 0x47060000:
        return "4706 Phy Test Function In Progress";
      case 0x477F0000:
        return "477f Some Commands Cleared By Iscsi Protocol Event";
      case 0x48000000:
        return "4800 Initiator Detected Error Message Received";
      case 0x49000000:
        return "4900 Invalid Message Error";
      case 0x4A000000:
        return "4a00 Command Phase Error";
      case 0x4B000000:
        return "4b00 Data Phase Error";
      case 0x4B010000:
        return "4b01 Invalid Target Port Transfer Tag Received";
      case 0x4B020000:
        return "4b02 Too Much Write Data";
      case 0x4B030000:
        return "4b03 Ack/Nak Timeout";
      case 0x4B040000:
        return "4b04 Nak Received";
      case 0x4B050000:
        return "4b05 Data Offset Error";
      case 0x4B060000:
        return "4b06 Initiator Response Timeout";
      case 0x4B070000:
        return "4b07 Connection Lost";
      case 0x4B080000:
        return "4b08 Data-In Buffer Overflow - Data Buffer Size";
      case 0x4B090000:
        return "4b09 Data-In Buffer Overflow - Data Buffer Descriptor Area";
      case 0x4B0A0000:
        return "4b0a Data-In Buffer Error";
      case 0x4B0B0000:
        return "4b0b Data-Out Buffer Overflow - Data Buffer Size";
      case 0x4B0C0000:
        return "4b0c Data-Out Buffer Overflow - Data Buffer Descriptor Area";
      case 0x4B0D0000:
        return "4b0d Data-Out Buffer Error";
      case 0x4B0E0000:
        return "4b0e Pcie Fabric Error";
      case 0x4B0F0000:
        return "4b0f Pcie Completion Timeout";
      case 0x4B100000:
        return "4b10 Pcie Completer Abort";
      case 0x4B110000:
        return "4b11 Pcie Poisoned Tlp Received";
      case 0x4B120000:
        return "4b12 Pcie Ecrc Check Failed";
      case 0x4B130000:
        return "4b13 Pcie Unsupported Request";
      case 0x4B140000:
        return "4b14 Pcie Acs Violation";
      case 0x4B150000:
        return "4b15 Pcie Tlp Prefix Blocked";
      case 0x4C000000:
        return "4c00 Logical Unit Failed Self-Configuration";
        // case 0x4DNN0000:
        //   return "4dnn Tagged Overlapped Commands (Nn = Task Tag)";
      case 0x4E000000:
        return "4e00 Overlapped Commands Attempted";
      case 0x4F000000:
        return "4f00";
      case 0x50000000:
        return "5000 Write Append Error";
      case 0x50010000:
        return "5001 Write Append Position Error";
      case 0x50020000:
        return "5002 Position Error Related To Timing";
      case 0x51000000:
        return "5100 Erase Failure";
      case 0x51010000:
        return "5101 Erase Failure - Incomplete Erase Operation Detected";
      case 0x52000000:
        return "5200 Cartridge Fault";
      case 0x53000000:
        return "5300 Media Load Or Eject Failed";
      case 0x53010000:
        return "5301 Unload Tape Failure";
      case 0x53020000:
        return "5302 Medium Removal Prevented";
      case 0x53030000:
        return "5303 Medium Removal Prevented By Data Transfer Element";
      case 0x53040000:
        return "5304 Medium Thread Or Unthread Failure";
      case 0x53050000:
        return "5305 Volume Identifier Invalid";
      case 0x53060000:
        return "5306 Volume Identifier Missing";
      case 0x53070000:
        return "5307 Duplicate Volume Identifier";
      case 0x53080000:
        return "5308 Element Status Unknown";
      case 0x53090000:
        return "5309 Data Transfer Device Error - Load Failed";
      case 0x530A0000:
        return "530a Data Transfer Device Error - Unload Failed";
      case 0x530B0000:
        return "530b Data Transfer Device Error - Unload Missing";
      case 0x530C0000:
        return "530c Data Transfer Device Error - Eject Failed";
      case 0x530D0000:
        return "530d Data Transfer Device Error - Library Communication Failed";
      case 0x54000000:
        return "5400 Scsi To Host System Interface Failure";
      case 0x55000000:
        return "5500 System Resource Failure";
      case 0x55010000:
        return "5501 System Buffer Full";
      case 0x55020000:
        return "5502 Insufficient Reservation Resources";
      case 0x55030000:
        return "5503 Insufficient Resources";
      case 0x55040000:
        return "5504 Insufficient Registration Resources";
      case 0x55050000:
        return "5505 Insufficient Access Control Resources";
      case 0x55060000:
        return "5506 Auxiliary Memory Out Of Space";
      case 0x55070000:
        return "5507 Quota Error";
      case 0x55080000:
        return "5508 Maximum Number Of Supplemental Decryption Keys Exceeded";
      case 0x55090000:
        return "5509 Medium Auxiliary Memory Not Accessible";
      case 0x550A0000:
        return "550a Data Currently Unavailable";
      case 0x550B0000:
        return "550b Insufficient Power For Operation";
      case 0x550C0000:
        return "550c Insufficient Resources To Create Rod";
      case 0x550D0000:
        return "550d Insufficient Resources To Create Rod Token";
      case 0x550E0000:
        return "550e Insufficient Zone Resources";
      case 0x550F0000:
        return "550f Insufficient Zone Resources To Complete Write";
      case 0x55100000:
        return "5510 Maximum Number Of Streams Open";
      case 0x55110000:
        return "5511 Insufficient Resources To Bind";
      case 0x56000000:
        return "5600";
      case 0x57000000:
        return "5700 Unable To Recover Table-Of-Contents";
      case 0x58000000:
        return "5800 Generation Does Not Exist";
      case 0x59000000:
        return "5900 Updated Block Read";
      case 0x5A000000:
        return "5a00 Operator Request Or State Change Input";
      case 0x5A010000:
        return "5a01 Operator Medium Removal Request";
      case 0x5A020000:
        return "5a02 Operator Selected Write Protect";
      case 0x5A030000:
        return "5a03 Operator Selected Write Permit";
      case 0x5B000000:
        return "5b00 Log Exception";
      case 0x5B010000:
        return "5b01 Threshold Condition Met";
      case 0x5B020000:
        return "5b02 Log Counter At Maximum";
      case 0x5B030000:
        return "5b03 Log List Codes Exhausted";
      case 0x5C000000:
        return "5c00 Rpl Status Change";
      case 0x5C010000:
        return "5c01 Spindles Synchronized";
      case 0x5C020000:
        return "5c02 Spindles Not Synchronized";
      case 0x5D000000:
        return "5d00 Failure Prediction Threshold Exceeded";
      case 0x5D010000:
        return "5d01 Media Failure Prediction Threshold Exceeded";
      case 0x5D020000:
        return "5d02 Logical Unit Failure Prediction Threshold Exceeded";
      case 0x5D030000:
        return "5d03 Spare Area Exhaustion Prediction Threshold Exceeded";
      case 0x5D100000:
        return "5d10 Hardware Impending Failure General Hard Drive Failure";
      case 0x5D110000:
        return "5d11 Hardware Impending Failure Drive Error Rate Too High";
      case 0x5D120000:
        return "5d12 Hardware Impending Failure Data Error Rate Too High";
      case 0x5D130000:
        return "5d13 Hardware Impending Failure Seek Error Rate Too High";
      case 0x5D140000:
        return "5d14 Hardware Impending Failure Too Many Block Reassigns";
      case 0x5D150000:
        return "5d15 Hardware Impending Failure Access Times Too High";
      case 0x5D160000:
        return "5d16 Hardware Impending Failure Start Unit Times Too High";
      case 0x5D170000:
        return "5d17 Hardware Impending Failure Channel Parametrics";
      case 0x5D180000:
        return "5d18 Hardware Impending Failure Controller Detected";
      case 0x5D190000:
        return "5d19 Hardware Impending Failure Throughput Performance";
      case 0x5D1A0000:
        return "5d1a Hardware Impending Failure Seek Time Performance";
      case 0x5D1B0000:
        return "5d1b Hardware Impending Failure Spin-Up Retry Count";
      case 0x5D1C0000:
        return "5d1c Hardware Impending Failure Drive Calibration Retry Count";
      case 0x5D200000:
        return "5d20 Controller Impending Failure General Hard Drive Failure";
      case 0x5D210000:
        return "5d21 Controller Impending Failure Drive Error Rate Too High";
      case 0x5D220000:
        return "5d22 Controller Impending Failure Data Error Rate Too High";
      case 0x5D230000:
        return "5d23 Controller Impending Failure Seek Error Rate Too High";
      case 0x5D240000:
        return "5d24 Controller Impending Failure Too Many Block Reassigns";
      case 0x5D250000:
        return "5d25 Controller Impending Failure Access Times Too High";
      case 0x5D260000:
        return "5d26 Controller Impending Failure Start Unit Times Too High";
      case 0x5D270000:
        return "5d27 Controller Impending Failure Channel Parametrics";
      case 0x5D280000:
        return "5d28 Controller Impending Failure Controller Detected";
      case 0x5D290000:
        return "5d29 Controller Impending Failure Throughput Performance";
      case 0x5D2A0000:
        return "5d2a Controller Impending Failure Seek Time Performance";
      case 0x5D2B0000:
        return "5d2b Controller Impending Failure Spin-Up Retry Count";
      case 0x5D2C0000:
        return "5d2c Controller Impending Failure Drive Calibration Retry Count";
      case 0x5D300000:
        return "5d30 Data Channel Impending Failure General Hard Drive Failure";
      case 0x5D310000:
        return "5d31 Data Channel Impending Failure Drive Error Rate Too High";
      case 0x5D320000:
        return "5d32 Data Channel Impending Failure Data Error Rate Too High";
      case 0x5D330000:
        return "5d33 Data Channel Impending Failure Seek Error Rate Too High";
      case 0x5D340000:
        return "5d34 Data Channel Impending Failure Too Many Block Reassigns";
      case 0x5D350000:
        return "5d35 Data Channel Impending Failure Access Times Too High";
      case 0x5D360000:
        return "5d36 Data Channel Impending Failure Start Unit Times Too High";
      case 0x5D370000:
        return "5d37 Data Channel Impending Failure Channel Parametrics";
      case 0x5D380000:
        return "5d38 Data Channel Impending Failure Controller Detected";
      case 0x5D390000:
        return "5d39 Data Channel Impending Failure Throughput Performance";
      case 0x5D3A0000:
        return "5d3a Data Channel Impending Failure Seek Time Performance";
      case 0x5D3B0000:
        return "5d3b Data Channel Impending Failure Spin-Up Retry Count";
      case 0x5D3C0000:
        return "5d3c Data Channel Impending Failure Drive Calibration Retry Count";
      case 0x5D400000:
        return "5d40 Servo Impending Failure General Hard Drive Failure";
      case 0x5D410000:
        return "5d41 Servo Impending Failure Drive Error Rate Too High";
      case 0x5D420000:
        return "5d42 Servo Impending Failure Data Error Rate Too High";
      case 0x5D430000:
        return "5d43 Servo Impending Failure Seek Error Rate Too High";
      case 0x5D440000:
        return "5d44 Servo Impending Failure Too Many Block Reassigns";
      case 0x5D450000:
        return "5d45 Servo Impending Failure Access Times Too High";
      case 0x5D460000:
        return "5d46 Servo Impending Failure Start Unit Times Too High";
      case 0x5D470000:
        return "5d47 Servo Impending Failure Channel Parametrics";
      case 0x5D480000:
        return "5d48 Servo Impending Failure Controller Detected";
      case 0x5D490000:
        return "5d49 Servo Impending Failure Throughput Performance";
      case 0x5D4A0000:
        return "5d4a Servo Impending Failure Seek Time Performance";
      case 0x5D4B0000:
        return "5d4b Servo Impending Failure Spin-Up Retry Count";
      case 0x5D4C0000:
        return "5d4c Servo Impending Failure Drive Calibration Retry Count";
      case 0x5D500000:
        return "5d50 Spindle Impending Failure General Hard Drive Failure";
      case 0x5D510000:
        return "5d51 Spindle Impending Failure Drive Error Rate Too High";
      case 0x5D520000:
        return "5d52 Spindle Impending Failure Data Error Rate Too High";
      case 0x5D530000:
        return "5d53 Spindle Impending Failure Seek Error Rate Too High";
      case 0x5D540000:
        return "5d54 Spindle Impending Failure Too Many Block Reassigns";
      case 0x5D550000:
        return "5d55 Spindle Impending Failure Access Times Too High";
      case 0x5D560000:
        return "5d56 Spindle Impending Failure Start Unit Times Too High";
      case 0x5D570000:
        return "5d57 Spindle Impending Failure Channel Parametrics";
      case 0x5D580000:
        return "5d58 Spindle Impending Failure Controller Detected";
      case 0x5D590000:
        return "5d59 Spindle Impending Failure Throughput Performance";
      case 0x5D5A0000:
        return "5d5a Spindle Impending Failure Seek Time Performance";
      case 0x5D5B0000:
        return "5d5b Spindle Impending Failure Spin-Up Retry Count";
      case 0x5D5C0000:
        return "5d5c Spindle Impending Failure Drive Calibration Retry Count";
      case 0x5D600000:
        return "5d60 Firmware Impending Failure General Hard Drive Failure";
      case 0x5D610000:
        return "5d61 Firmware Impending Failure Drive Error Rate Too High";
      case 0x5D620000:
        return "5d62 Firmware Impending Failure Data Error Rate Too High";
      case 0x5D630000:
        return "5d63 Firmware Impending Failure Seek Error Rate Too High";
      case 0x5D640000:
        return "5d64 Firmware Impending Failure Too Many Block Reassigns";
      case 0x5D650000:
        return "5d65 Firmware Impending Failure Access Times Too High";
      case 0x5D660000:
        return "5d66 Firmware Impending Failure Start Unit Times Too High";
      case 0x5D670000:
        return "5d67 Firmware Impending Failure Channel Parametrics";
      case 0x5D680000:
        return "5d68 Firmware Impending Failure Controller Detected";
      case 0x5D690000:
        return "5d69 Firmware Impending Failure Throughput Performance";
      case 0x5D6A0000:
        return "5d6a Firmware Impending Failure Seek Time Performance";
      case 0x5D6B0000:
        return "5d6b Firmware Impending Failure Spin-Up Retry Count";
      case 0x5D6C0000:
        return "5d6c Firmware Impending Failure Drive Calibration Retry Count";
      case 0x5DFF0000:
        return "5dff Failure Prediction Threshold Exceeded (False)";
      case 0x5E000000:
        return "5e00 Low Power Condition On";
      case 0x5E010000:
        return "5e01 Idle Condition Activated By Timer";
      case 0x5E020000:
        return "5e02 Standby Condition Activated By Timer";
      case 0x5E030000:
        return "5e03 Idle Condition Activated By Command";
      case 0x5E040000:
        return "5e04 Standby Condition Activated By Command";
      case 0x5E050000:
        return "5e05 Idle_b Condition Activated By Timer";
      case 0x5E060000:
        return "5e06 Idle_b Condition Activated By Command";
      case 0x5E070000:
        return "5e07 Idle_c Condition Activated By Timer";
      case 0x5E080000:
        return "5e08 Idle_c Condition Activated By Command";
      case 0x5E090000:
        return "5e09 Standby_y Condition Activated By Timer";
      case 0x5E0A0000:
        return "5e0a Standby_y Condition Activated By Command";
      case 0x5E410000:
        return "5e41 Power State Change To Active";
      case 0x5E420000:
        return "5e42 Power State Change To Idle";
      case 0x5E430000:
        return "5e43 Power State Change To Standby";
      case 0x5E450000:
        return "5e45 Power State Change To Sleep";
      case 0x5E470000:
        return "5e47 Power State Change To Device Control";
      case 0x5F000000:
        return "5f00";
      case 0x60000000:
        return "6000 Lamp Failure";
      case 0x61000000:
        return "6100 Video Acquisition Error";
      case 0x61010000:
        return "6101 Unable To Acquire Video";
      case 0x61020000:
        return "6102 Out Of Focus";
      case 0x62000000:
        return "6200 Scan Head Positioning Error";
      case 0x63000000:
        return "6300 End Of User Area Encountered On This Track";
      case 0x63010000:
        return "6301 Packet Does Not Fit In Available Space";
      case 0x64000000:
        return "6400 Illegal Mode For This Track";
      case 0x64010000:
        return "6401 Invalid Packet Size";
      case 0x65000000:
        return "6500 Voltage Fault";
      case 0x66000000:
        return "6600 Automatic Document Feeder Cover Up";
      case 0x66010000:
        return "6601 Automatic Document Feeder Lift Up";
      case 0x66020000:
        return "6602 Document Jam In Automatic Document Feeder";
      case 0x66030000:
        return "6603 Document Miss Feed Automatic In Document Feeder";
      case 0x67000000:
        return "6700 Configuration Failure";
      case 0x67010000:
        return "6701 Configuration Of Incapable Logical Units Failed";
      case 0x67020000:
        return "6702 Add Logical Unit Failed";
      case 0x67030000:
        return "6703 Modification Of Logical Unit Failed";
      case 0x67040000:
        return "6704 Exchange Of Logical Unit Failed";
      case 0x67050000:
        return "6705 Remove Of Logical Unit Failed";
      case 0x67060000:
        return "6706 Attachment Of Logical Unit Failed";
      case 0x67070000:
        return "6707 Creation Of Logical Unit Failed";
      case 0x67080000:
        return "6708 Assign Failure Occurred";
      case 0x67090000:
        return "6709 Multiply Assigned Logical Unit";
      case 0x670A0000:
        return "670a Set Target Port Groups Command Failed";
      case 0x670B0000:
        return "670b Ata Device Feature Not Enabled";
      case 0x670C0000:
        return "670c Command Rejected";
      case 0x670D0000:
        return "670d Explicit Bind Not Allowed";
      case 0x68000000:
        return "6800 Logical Unit Not Configured";
      case 0x68010000:
        return "6801 Subsidiary Logical Unit Not Configured";
      case 0x69000000:
        return "6900 Data Loss On Logical Unit";
      case 0x69010000:
        return "6901 Multiple Logical Unit Failures";
      case 0x69020000:
        return "6902 Parity/Data Mismatch";
      case 0x6A000000:
        return "6a00 Informational, Refer To Log";
      case 0x6B000000:
        return "6b00 State Change Has Occurred";
      case 0x6B010000:
        return "6b01 Redundancy Level Got Better";
      case 0x6B020000:
        return "6b02 Redundancy Level Got Worse";
      case 0x6C000000:
        return "6c00 Rebuild Failure Occurred";
      case 0x6D000000:
        return "6d00 Recalculate Failure Occurred";
      case 0x6E000000:
        return "6e00 Command To Logical Unit Failed";
      case 0x6F000000:
        return "6f00 Copy Protection Key Exchange Failure - Authentication Failure";
      case 0x6F010000:
        return "6f01 Copy Protection Key Exchange Failure - Key Not Present";
      case 0x6F020000:
        return "6f02 Copy Protection Key Exchange Failure - Key Not Established";
      case 0x6F030000:
        return "6f03 Read Of Scrambled Sector Without Authentication";
      case 0x6F040000:
        return "6f04 Media Region Code Is Mismatched To Logical Unit Region";
      case 0x6F050000:
        return "6f05 Drive Region Must Be Permanent/Region Reset Count Error";
      case 0x6F060000:
        return "6f06 Insufficient Block Count For Binding Nonce Recording";
      case 0x6F070000:
        return "6f07 Conflict In Binding Nonce Recording";
      case 0x6F080000:
        return "6f08 Insufficient Permission";
      case 0x6F090000:
        return "6f09 Invalid Drive-Host Pairing Server";
      case 0x6F0A0000:
        return "6f0a Drive-Host Pairing Suspended";
        // case 0x70NN0000:
        //   return "70nn Decompression Exception Short Algorithm Id Of Nn";
      case 0x71000000:
        return "7100 Decompression Exception Long Algorithm Id";
      case 0x72000000:
        return "7200 Session Fixation Error";
      case 0x72010000:
        return "7201 Session Fixation Error Writing Lead-In";
      case 0x72020000:
        return "7202 Session Fixation Error Writing Lead-Out";
      case 0x72030000:
        return "7203 Session Fixation Error - Incomplete Track In Session";
      case 0x72040000:
        return "7204 Empty Or Partially Written Reserved Track";
      case 0x72050000:
        return "7205 No More Track Reservations Allowed";
      case 0x72060000:
        return "7206 Rmz Extension Is Not Allowed";
      case 0x72070000:
        return "7207 No More Test Zone Extensions Are Allowed";
      case 0x73000000:
        return "7300 Cd Control Error";
      case 0x73010000:
        return "7301 Power Calibration Area Almost Full";
      case 0x73020000:
        return "7302 Power Calibration Area Is Full";
      case 0x73030000:
        return "7303 Power Calibration Area Error";
      case 0x73040000:
        return "7304 Program Memory Area Update Failure";
      case 0x73050000:
        return "7305 Program Memory Area Is Full";
      case 0x73060000:
        return "7306 Rma/Pma Is Almost Full";
      case 0x73100000:
        return "7310 Current Power Calibration Area Almost Full";
      case 0x73110000:
        return "7311 Current Power Calibration Area Is Full";
      case 0x73170000:
        return "7317 Rdz Is Full";
      case 0x74000000:
        return "7400 Security Error";
      case 0x74010000:
        return "7401 Unable To Decrypt Data";
      case 0x74020000:
        return "7402 Unencrypted Data Encountered While Decrypting";
      case 0x74030000:
        return "7403 Incorrect Data Encryption Key";
      case 0x74040000:
        return "7404 Cryptographic Integrity Validation Failed";
      case 0x74050000:
        return "7405 Error Decrypting Data";
      case 0x74060000:
        return "7406 Unknown Signature Verification Key";
      case 0x74070000:
        return "7407 Encryption Parameters Not Useable";
      case 0x74080000:
        return "7408 Digital Signature Validation Failure";
      case 0x74090000:
        return "7409 Encryption Mode Mismatch On Read";
      case 0x740A0000:
        return "740a Encrypted Block Not Raw Read Enabled";
      case 0x740B0000:
        return "740b Incorrect Encryption Parameters";
      case 0x740C0000:
        return "740c Unable To Decrypt Parameter List";
      case 0x740D0000:
        return "740d Encryption Algorithm Disabled";
      case 0x74100000:
        return "7410 Sa Creation Parameter Value Invalid";
      case 0x74110000:
        return "7411 Sa Creation Parameter Value Rejected";
      case 0x74120000:
        return "7412 Invalid Sa Usage";
      case 0x74210000:
        return "7421 Data Encryption Configuration Prevented";
      case 0x74300000:
        return "7430 Sa Creation Parameter Not Supported";
      case 0x74400000:
        return "7440 Authentication Failed";
      case 0x74610000:
        return "7461 External Data Encryption Key Manager Access Error";
      case 0x74620000:
        return "7462 External Data Encryption Key Manager Error";
      case 0x74630000:
        return "7463 External Data Encryption Key Not Found";
      case 0x74640000:
        return "7464 External Data Encryption Request Not Authorized";
      case 0x746E0000:
        return "746e External Data Encryption Control Timeout";
      case 0x746F0000:
        return "746f External Data Encryption Control Error";
      case 0x74710000:
        return "7471 Logical Unit Access Not Authorized";
      case 0x74790000:
        return "7479 Security Conflict In Translated Device";
      case 0x75000000:
        return "7500";
      case 0x76000000:
        return "7600";
      case 0x77000000:
        return "7700";
      case 0x78000000:
        return "7800";
      case 0x79000000:
        return "7900";
      case 0x7A000000:
        return "7a00";
      case 0x7B000000:
        return "7b00";
      case 0x7C000000:
        return "7c00";
      case 0x7D000000:
        return "7d00";
      case 0x7E000000:
        return "7e00";
      case 0x7F000000:
        return "7f00";

      default:
        return "::UNKNOWN::";
      }
  }

  int
  asc_ascq_to_errno(const uint8_t asc,
                    const uint8_t ascq)
  {
    return ((asc << 24) | (ascq << 16));
  }

  int
  asc_ascq_to_errno(const uint8_t * const sb)
  {
    uint8_t asc  = additional_sense_code(sb);
    uint8_t ascq = additional_sense_code_qualifier(sb);

    return asc_ascq_to_errno(asc,ascq);
  }
}
