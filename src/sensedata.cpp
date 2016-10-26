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
      case 0x00000100:
        return "0001 Filemark Detected";
      case 0x00000200:
        return "0002 End-Of-Partition/Medium Detected";
      case 0x00000300:
        return "0003 Setmark Detected";
      case 0x00000400:
        return "0004 Beginning-Of-Partition/Medium Detected";
      case 0x00000500:
        return "0005 End-Of-Data Detected";
      case 0x00000600:
        return "0006 I/O Process Terminated";
      case 0x00000700:
        return "0007 Programmable Early Warning Detected";
      case 0x00001100:
        return "0011 Audio Play Operation In Progress";
      case 0x00001200:
        return "0012 Audio Play Operation Paused";
      case 0x00001300:
        return "0013 Audio Play Operation Successfully Completed";
      case 0x00001400:
        return "0014 Audio Play Operation Stopped Due To Error";
      case 0x00001500:
        return "0015 No Current Audio Status To Return";
      case 0x00001600:
        return "0016 Operation In Progress";
      case 0x00001700:
        return "0017 Cleaning Requested";
      case 0x00001800:
        return "0018 Erase Operation In Progress";
      case 0x00001900:
        return "0019 Locate Operation In Progress";
      case 0x00001A00:
        return "001a Rewind Operation In Progress";
      case 0x00001B00:
        return "001b Set Capacity Operation In Progress";
      case 0x00001C00:
        return "001c Verify Operation In Progress";
      case 0x00001D00:
        return "001d Ata Pass Through Information Available";
      case 0x00001E00:
        return "001e Conflicting Sa Creation Request";
      case 0x00001F00:
        return "001f Logical Unit Transitioning To Another Power Condition";
      case 0x00002000:
        return "0020 Extended Copy Information Available";
      case 0x00002100:
        return "0021 Atomic Command Aborted Due To Aca";
      case 0x00010000:
        return "0100 No Index/Sector Signal";
      case 0x00020000:
        return "0200 No Seek Complete";
      case 0x00030000:
        return "0300 Peripheral Device Write Fault";
      case 0x00030100:
        return "0301 No Write Current";
      case 0x00030200:
        return "0302 Excessive Write Errors";
      case 0x00040000:
        return "0400 Logical Unit Not Ready, Cause Not Reportable";
      case 0x00040100:
        return "0401 Logical Unit Is In Process Of Becoming Ready";
      case 0x00040200:
        return "0402 Logical Unit Not Ready, Initializing Command Required";
      case 0x00040300:
        return "0403 Logical Unit Not Ready, Manual Intervention Required";
      case 0x00040400:
        return "0404 Logical Unit Not Ready, Format In Progress";
      case 0x00040500:
        return "0405 Logical Unit Not Ready, Rebuild In Progress";
      case 0x00040600:
        return "0406 Logical Unit Not Ready, Recalculation In Progress";
      case 0x00040700:
        return "0407 Logical Unit Not Ready, Operation In Progress";
      case 0x00040800:
        return "0408 Logical Unit Not Ready, Long Write In Progress";
      case 0x00040900:
        return "0409 Logical Unit Not Ready, Self-Test In Progress";
      case 0x00040A00:
        return "040a Logical Unit Not Accessible, Asymmetric Access State Transition";
      case 0x00040B00:
        return "040b Logical Unit Not Accessible, Target Port In Standby State";
      case 0x00040C00:
        return "040c Logical Unit Not Accessible, Target Port In Unavailable State";
      case 0x00040D00:
        return "040d Logical Unit Not Ready, Structure Check Required";
      case 0x00040E00:
        return "040e Logical Unit Not Ready, Security Session In Progress";
      case 0x00041000:
        return "0410 Logical Unit Not Ready, Auxiliary Memory Not Accessible";
      case 0x00041100:
        return "0411 Logical Unit Not Ready, Notify (Enable Spinup) Required";
      case 0x00041200:
        return "0412 Logical Unit Not Ready, Offline";
      case 0x00041300:
        return "0413 Logical Unit Not Ready, Sa Creation In Progress";
      case 0x00041400:
        return "0414 Logical Unit Not Ready, Space Allocation In Progress";
      case 0x00041500:
        return "0415 Logical Unit Not Ready, Robotics Disabled";
      case 0x00041600:
        return "0416 Logical Unit Not Ready, Configuration Required";
      case 0x00041700:
        return "0417 Logical Unit Not Ready, Calibration Required";
      case 0x00041800:
        return "0418 Logical Unit Not Ready, A Door Is Open";
      case 0x00041900:
        return "0419 Logical Unit Not Ready, Operating In Sequential Mode";
      case 0x00041A00:
        return "041a Logical Unit Not Ready, Start Stop Unit Command In Progress";
      case 0x00041B00:
        return "041b Logical Unit Not Ready, Sanitize In Progress";
      case 0x00041C00:
        return "041c Logical Unit Not Ready, Additional Power Use Not Yet Granted";
      case 0x00041D00:
        return "041d Logical Unit Not Ready, Configuration In Progress";
      case 0x00041E00:
        return "041e Logical Unit Not Ready, Microcode Activation Required";
      case 0x00041F00:
        return "041f Logical Unit Not Ready, Microcode Download Required";
      case 0x00042000:
        return "0420 Logical Unit Not Ready, Logical Unit Reset Required";
      case 0x00042100:
        return "0421 Logical Unit Not Ready, Hard Reset Required";
      case 0x00042200:
        return "0422 Logical Unit Not Ready, Power Cycle Required";
      case 0x00042300:
        return "0423 Logical Unit Not Ready, Affiliation Required";
      case 0x00050000:
        return "0500 Logical Unit Does Not Respond To Selection";
      case 0x00060000:
        return "0600 No Reference Position Found";
      case 0x00070000:
        return "0700 Multiple Peripheral Devices Selected";
      case 0x00080000:
        return "0800 Logical Unit Communication Failure";
      case 0x00080100:
        return "0801 Logical Unit Communication Time-Out";
      case 0x00080200:
        return "0802 Logical Unit Communication Parity Error";
      case 0x00080300:
        return "0803 Logical Unit Communication Crc Error (Ultra-Dma/32)";
      case 0x00080400:
        return "0804 Unreachable Copy Target";
      case 0x00090000:
        return "0900 Track Following Error";
      case 0x00090100:
        return "0901 Tracking Servo Failure";
      case 0x00090200:
        return "0902 Focus Servo Failure";
      case 0x00090300:
        return "0903 Spindle Servo Failure";
      case 0x00090400:
        return "0904 Head Select Fault";
      case 0x00090500:
        return "0905 Vibration Induced Tracking Error";
      case 0x000A0000:
        return "0a00 Error Log Overflow";
      case 0x000B0000:
        return "0b00 Warning";
      case 0x000B0100:
        return "0b01 Warning - Specified Temperature Exceeded";
      case 0x000B0200:
        return "0b02 Warning - Enclosure Degraded";
      case 0x000B0300:
        return "0b03 Warning - Background Self-Test Failed";
      case 0x000B0400:
        return "0b04 Warning - Background Pre-Scan Detected Medium Error";
      case 0x000B0500:
        return "0b05 Warning - Background Medium Scan Detected Medium Error";
      case 0x000B0600:
        return "0b06 Warning - Non-Volatile Cache Now Volatile";
      case 0x000B0700:
        return "0b07 Warning - Degraded Power To Non-Volatile Cache";
      case 0x000B0800:
        return "0b08 Warning - Power Loss Expected";
      case 0x000B0900:
        return "0b09 Warning - Device Statistics Notification Active";
      case 0x000B0A00:
        return "0b0a Warning - High Critical Temperature Limit Exceeded";
      case 0x000B0B00:
        return "0b0b Warning - Low Critical Temperature Limit Exceeded";
      case 0x000B0C00:
        return "0b0c Warning - High Operating Temperature Limit Exceeded";
      case 0x000B0D00:
        return "0b0d Warning - Low Operating Temperature Limit Exceeded";
      case 0x000B0E00:
        return "0b0e Warning - High Critical Humidity Limit Exceeded";
      case 0x000B0F00:
        return "0b0f Warning - Low Critical Humidity Limit Exceeded";
      case 0x000B1000:
        return "0b10 Warning - High Operating Humidity Limit Exceeded";
      case 0x000B1100:
        return "0b11 Warning - Low Operating Humidity Limit Exceeded";
      case 0x000C0000:
        return "0c00 Write Error";
      case 0x000C0100:
        return "0c01 Write Error - Recovered With Auto Reallocation";
      case 0x000C0200:
        return "0c02 Write Error - Auto Reallocation Failed";
      case 0x000C0300:
        return "0c03 Write Error - Recommend Reassignment";
      case 0x000C0400:
        return "0c04 Compression Check Miscompare Error";
      case 0x000C0500:
        return "0c05 Data Expansion Occurred During Compression";
      case 0x000C0600:
        return "0c06 Block Not Compressible";
      case 0x000C0700:
        return "0c07 Write Error - Recovery Needed";
      case 0x000C0800:
        return "0c08 Write Error - Recovery Failed";
      case 0x000C0900:
        return "0c09 Write Error - Loss Of Streaming";
      case 0x000C0A00:
        return "0c0a Write Error - Padding Blocks Added";
      case 0x000C0B00:
        return "0c0b Auxiliary Memory Write Error";
      case 0x000C0C00:
        return "0c0c Write Error - Unexpected Unsolicited Data";
      case 0x000C0D00:
        return "0c0d Write Error - Not Enough Unsolicited Data";
      case 0x000C0E00:
        return "0c0e Multiple Write Errors";
      case 0x000C0F00:
        return "0c0f Defects In Error Window";
      case 0x000C1000:
        return "0c10 Incomplete Multiple Atomic Write Operations";
      case 0x000C1100:
        return "0c11 Write Error - Recovery Scan Needed";
      case 0x000C1200:
        return "0c12 Write Error - Insufficient Zone Resources";
      case 0x000D0000:
        return "0d00 Error Detected By Third Party Temporary Initiator";
      case 0x000D0100:
        return "0d01 Third Party Device Failure";
      case 0x000D0200:
        return "0d02 Copy Target Device Not Reachable";
      case 0x000D0300:
        return "0d03 Incorrect Copy Target Device Type";
      case 0x000D0400:
        return "0d04 Copy Target Device Data Underrun";
      case 0x000D0500:
        return "0d05 Copy Target Device Data Overrun";
      case 0x000E0000:
        return "0e00 Invalid Information Unit";
      case 0x000E0100:
        return "0e01 Information Unit Too Short";
      case 0x000E0200:
        return "0e02 Information Unit Too Long";
      case 0x000E0300:
        return "0e03 Invalid Field In Command Information Unit";
      case 0x000F0000:
        return "0f00";
      case 0x00100000:
        return "1000 Id Crc Or Ecc Error";
      case 0x00100100:
        return "1001 Logical Block Guard Check Failed";
      case 0x00100200:
        return "1002 Logical Block Application Tag Check Failed";
      case 0x00100300:
        return "1003 Logical Block Reference Tag Check Failed";
      case 0x00100400:
        return "1004 Logical Block Protection Error On Recover Buffered Data";
      case 0x00100500:
        return "1005 Logical Block Protection Method Error";
      case 0x00110000:
        return "1100 Unrecovered Read Error";
      case 0x00110100:
        return "1101 Read Retries Exhausted";
      case 0x00110200:
        return "1102 Error Too Long To Correct";
      case 0x00110300:
        return "1103 Multiple Read Errors";
      case 0x00110400:
        return "1104 Unrecovered Read Error - Auto Reallocate Failed";
      case 0x00110500:
        return "1105 L-Ec Uncorrectable Error";
      case 0x00110600:
        return "1106 Circ Unrecovered Error";
      case 0x00110700:
        return "1107 Data Re-Synchronization Error";
      case 0x00110800:
        return "1108 Incomplete Block Read";
      case 0x00110900:
        return "1109 No Gap Found";
      case 0x00110A00:
        return "110a Miscorrected Error";
      case 0x00110B00:
        return "110b Unrecovered Read Error - Recommend Reassignment";
      case 0x00110C00:
        return "110c Unrecovered Read Error - Recommend Rewrite The Data";
      case 0x00110D00:
        return "110d De-Compression Crc Error";
      case 0x00110E00:
        return "110e Cannot Decompress Using Declared Algorithm";
      case 0x00110F00:
        return "110f Error Reading Upc/Ean Number";
      case 0x00111000:
        return "1110 Error Reading Isrc Number";
      case 0x00111100:
        return "1111 Read Error - Loss Of Streaming";
      case 0x00111200:
        return "1112 Auxiliary Memory Read Error";
      case 0x00111300:
        return "1113 Read Error - Failed Retransmission Request";
      case 0x00111400:
        return "1114 Read Error - Lba Marked Bad By Application Client";
      case 0x00111500:
        return "1115 Write After Sanitize Required";
      case 0x00120000:
        return "1200 Address Mark Not Found For Id Field";
      case 0x00130000:
        return "1300 Address Mark Not Found For Data Field";
      case 0x00140000:
        return "1400 Recorded Entity Not Found";
      case 0x00140100:
        return "1401 Record Not Found";
      case 0x00140200:
        return "1402 Filemark Or Setmark Not Found";
      case 0x00140300:
        return "1403 End-Of-Data Not Found";
      case 0x00140400:
        return "1404 Block Sequence Error";
      case 0x00140500:
        return "1405 Record Not Found - Recommend Reassignment";
      case 0x00140600:
        return "1406 Record Not Found - Data Auto-Reallocated";
      case 0x00140700:
        return "1407 Locate Operation Failure";
      case 0x00150000:
        return "1500 Random Positioning Error";
      case 0x00150100:
        return "1501 Mechanical Positioning Error";
      case 0x00150200:
        return "1502 Positioning Error Detected By Read Of Medium";
      case 0x00160000:
        return "1600 Data Synchronization Mark Error";
      case 0x00160100:
        return "1601 Data Sync Error - Data Rewritten";
      case 0x00160200:
        return "1602 Data Sync Error - Recommend Rewrite";
      case 0x00160300:
        return "1603 Data Sync Error - Data Auto-Reallocated";
      case 0x00160400:
        return "1604 Data Sync Error - Recommend Reassignment";
      case 0x00170000:
        return "1700 Recovered Data With No Error Correction Applied";
      case 0x00170100:
        return "1701 Recovered Data With Retries";
      case 0x00170200:
        return "1702 Recovered Data With Positive Head Offset";
      case 0x00170300:
        return "1703 Recovered Data With Negative Head Offset";
      case 0x00170400:
        return "1704 Recovered Data With Retries And/Or Circ Applied";
      case 0x00170500:
        return "1705 Recovered Data Using Previous Sector Id";
      case 0x00170600:
        return "1706 Recovered Data Without Ecc - Data Auto-Reallocated";
      case 0x00170700:
        return "1707 Recovered Data Without Ecc - Recommend Reassignment";
      case 0x00170800:
        return "1708 Recovered Data Without Ecc - Recommend Rewrite";
      case 0x00170900:
        return "1709 Recovered Data Without Ecc - Data Rewritten";
      case 0x00180000:
        return "1800 Recovered Data With Error Correction Applied";
      case 0x00180100:
        return "1801 Recovered Data With Error Corr. & Retries Applied";
      case 0x00180200:
        return "1802 Recovered Data - Data Auto-Reallocated";
      case 0x00180300:
        return "1803 Recovered Data With Circ";
      case 0x00180400:
        return "1804 Recovered Data With L-Ec";
      case 0x00180500:
        return "1805 Recovered Data - Recommend Reassignment";
      case 0x00180600:
        return "1806 Recovered Data - Recommend Rewrite";
      case 0x00180700:
        return "1807 Recovered Data With Ecc - Data Rewritten";
      case 0x00180800:
        return "1808 Recovered Data With Linking";
      case 0x00190000:
        return "1900 Defect List Error";
      case 0x00190100:
        return "1901 Defect List Not Available";
      case 0x00190200:
        return "1902 Defect List Error In Primary List";
      case 0x00190300:
        return "1903 Defect List Error In Grown List";
      case 0x001A0000:
        return "1a00 Parameter List Length Error";
      case 0x001B0000:
        return "1b00 Synchronous Data Transfer Error";
      case 0x001C0000:
        return "1c00 Defect List Not Found";
      case 0x001C0100:
        return "1c01 Primary Defect List Not Found";
      case 0x001C0200:
        return "1c02 Grown Defect List Not Found";
      case 0x001D0000:
        return "1d00 Miscompare During Verify Operation";
      case 0x001D0100:
        return "1d01 Miscompare Verify Of Unmapped Lba";
      case 0x001E0000:
        return "1e00 Recovered Id With Ecc Correction";
      case 0x001F0000:
        return "1f00 Partial Defect List Transfer";
      case 0x00200000:
        return "2000 Invalid Command Operation Code";
      case 0x00200100:
        return "2001 Access Denied - Initiator Pending-Enrolled";
      case 0x00200200:
        return "2002 Access Denied - No Access Rights";
      case 0x00200300:
        return "2003 Access Denied - Invalid Mgmt Id Key";
      case 0x00200400:
        return "2004 Illegal Command While In Write Capable State";
      case 0x00200500:
        return "2005 Obsolete";
      case 0x00200600:
        return "2006 Illegal Command While In Explicit Address Mode";
      case 0x00200700:
        return "2007 Illegal Command While In Implicit Address Mode";
      case 0x00200800:
        return "2008 Access Denied - Enrollment Conflict";
      case 0x00200900:
        return "2009 Access Denied - Invalid Lu Identifier";
      case 0x00200A00:
        return "200a Access Denied - Invalid Proxy Token";
      case 0x00200B00:
        return "200b Access Denied - Acl Lun Conflict";
      case 0x00200C00:
        return "200c Illegal Command When Not In Append-Only Mode";
      case 0x00200D00:
        return "200d Not An Administrative Logical Unit";
      case 0x00200E00:
        return "200e Not A Subsidiary Logical Unit";
      case 0x00200F00:
        return "200f Not A Conglomerate Logical Unit";
      case 0x00210000:
        return "2100 Logical Block Address Out Of Range";
      case 0x00210100:
        return "2101 Invalid Element Address";
      case 0x00210200:
        return "2102 Invalid Address For Write";
      case 0x00210300:
        return "2103 Invalid Write Crossing Layer Jump";
      case 0x00210400:
        return "2104 Unaligned Write Command";
      case 0x00210500:
        return "2105 Write Boundary Violation";
      case 0x00210600:
        return "2106 Attempt To Read Invalid Data";
      case 0x00210700:
        return "2107 Read Boundary Violation";
      case 0x00210800:
        return "2108 Misaligned Write Command";
      case 0x00220000:
        return "2200 Illegal Function (Use 20 00, 24 00, Or 26 00)";
      case 0x00230000:
        return "2300 Invalid Token Operation, Cause Not Reportable";
      case 0x00230100:
        return "2301 Invalid Token Operation, Unsupported Token Type";
      case 0x00230200:
        return "2302 Invalid Token Operation, Remote Token Usage Not Supported";
      case 0x00230300:
        return "2303 Invalid Token Operation, Remote Rod Token Creation Not Supported";
      case 0x00230400:
        return "2304 Invalid Token Operation, Token Unknown";
      case 0x00230500:
        return "2305 Invalid Token Operation, Token Corrupt";
      case 0x00230600:
        return "2306 Invalid Token Operation, Token Revoked";
      case 0x00230700:
        return "2307 Invalid Token Operation, Token Expired";
      case 0x00230800:
        return "2308 Invalid Token Operation, Token Cancelled";
      case 0x00230900:
        return "2309 Invalid Token Operation, Token Deleted";
      case 0x00230A00:
        return "230a Invalid Token Operation, Invalid Token Length";
      case 0x00240000:
        return "2400 Invalid Field In Cdb";
      case 0x00240100:
        return "2401 Cdb Decryption Error";
      case 0x00240200:
        return "2402 Obsolete";
      case 0x00240300:
        return "2403 Obsolete";
      case 0x00240400:
        return "2404 Security Audit Value Frozen";
      case 0x00240500:
        return "2405 Security Working Key Frozen";
      case 0x00240600:
        return "2406 Nonce Not Unique";
      case 0x00240700:
        return "2407 Nonce Timestamp Out Of Range";
      case 0x00240800:
        return "2408 Invalid Xcdb";
      case 0x00240900:
        return "2409 Invalid Fast Format";
      case 0x00250000:
        return "2500 Logical Unit Not Supported";
      case 0x00260000:
        return "2600 Invalid Field In Parameter List";
      case 0x00260100:
        return "2601 Parameter Not Supported";
      case 0x00260200:
        return "2602 Parameter Value Invalid";
      case 0x00260300:
        return "2603 Threshold Parameters Not Supported";
      case 0x00260400:
        return "2604 Invalid Release Of Persistent Reservation";
      case 0x00260500:
        return "2605 Data Decryption Error";
      case 0x00260600:
        return "2606 Too Many Target Descriptors";
      case 0x00260700:
        return "2607 Unsupported Target Descriptor Type Code";
      case 0x00260800:
        return "2608 Too Many Segment Descriptors";
      case 0x00260900:
        return "2609 Unsupported Segment Descriptor Type Code";
      case 0x00260A00:
        return "260a Unexpected Inexact Segment";
      case 0x00260B00:
        return "260b Inline Data Length Exceeded";
      case 0x00260C00:
        return "260c Invalid Operation For Copy Source Or Destination";
      case 0x00260D00:
        return "260d Copy Segment Granularity Violation";
      case 0x00260E00:
        return "260e Invalid Parameter While Port Is Enabled";
      case 0x00260F00:
        return "260f Invalid Data-Out Buffer Integrity Check Value";
      case 0x00261000:
        return "2610 Data Decryption Key Fail Limit Reached";
      case 0x00261100:
        return "2611 Incomplete Key-Associated Data Set";
      case 0x00261200:
        return "2612 Vendor Specific Key Reference Not Found";
      case 0x00261300:
        return "2613 Application Tag Mode Page Is Invalid";
      case 0x00261400:
        return "2614 Tape Stream Mirroring Prevented";
      case 0x00261500:
        return "2615 Copy Source Or Copy Destination Not Authorized";
      case 0x00270000:
        return "2700 Write Protected";
      case 0x00270100:
        return "2701 Hardware Write Protected";
      case 0x00270200:
        return "2702 Logical Unit Software Write Protected";
      case 0x00270300:
        return "2703 Associated Write Protect";
      case 0x00270400:
        return "2704 Persistent Write Protect";
      case 0x00270500:
        return "2705 Permanent Write Protect";
      case 0x00270600:
        return "2706 Conditional Write Protect";
      case 0x00270700:
        return "2707 Space Allocation Failed Write Protect";
      case 0x00270800:
        return "2708 Zone Is Read Only";
      case 0x00280000:
        return "2800 Not Ready To Ready Change, Medium May Have Changed";
      case 0x00280100:
        return "2801 Import Or Export Element Accessed";
      case 0x00280200:
        return "2802 Format-Layer May Have Changed";
      case 0x00280300:
        return "2803 Import/Export Element Accessed, Medium Changed";
      case 0x00290000:
        return "2900 Power On, Reset, Or Bus Device Reset Occurred";
      case 0x00290100:
        return "2901 Power On Occurred";
      case 0x00290200:
        return "2902 Scsi Bus Reset Occurred";
      case 0x00290300:
        return "2903 Bus Device Reset Function Occurred";
      case 0x00290400:
        return "2904 Device Internal Reset";
      case 0x00290500:
        return "2905 Transceiver Mode Changed To Single-Ended";
      case 0x00290600:
        return "2906 Transceiver Mode Changed To Lvd";
      case 0x00290700:
        return "2907 I_t Nexus Loss Occurred";
      case 0x002A0000:
        return "2a00 Parameters Changed";
      case 0x002A0100:
        return "2a01 Mode Parameters Changed";
      case 0x002A0200:
        return "2a02 Log Parameters Changed";
      case 0x002A0300:
        return "2a03 Reservations Preempted";
      case 0x002A0400:
        return "2a04 Reservations Released";
      case 0x002A0500:
        return "2a05 Registrations Preempted";
      case 0x002A0600:
        return "2a06 Asymmetric Access State Changed";
      case 0x002A0700:
        return "2a07 Implicit Asymmetric Access State Transition Failed";
      case 0x002A0800:
        return "2a08 Priority Changed";
      case 0x002A0900:
        return "2a09 Capacity Data Has Changed";
      case 0x002A0A00:
        return "2a0a Error History I_t Nexus Cleared";
      case 0x002A0B00:
        return "2a0b Error History Snapshot Released";
      case 0x002A0C00:
        return "2a0c Error Recovery Attributes Have Changed";
      case 0x002A0D00:
        return "2a0d Data Encryption Capabilities Changed";
      case 0x002A1000:
        return "2a10 Timestamp Changed";
      case 0x002A1100:
        return "2a11 Data Encryption Parameters Changed By Another I_t Nexus";
      case 0x002A1200:
        return "2a12 Data Encryption Parameters Changed By Vendor Specific Event";
      case 0x002A1300:
        return "2a13 Data Encryption Key Instance Counter Has Changed";
      case 0x002A1400:
        return "2a14 Sa Creation Capabilities Data Has Changed";
      case 0x002A1500:
        return "2a15 Medium Removal Prevention Preempted";
      case 0x002A1600:
        return "2a16 Zone Reset Write Pointer Recommended";
      case 0x002B0000:
        return "2b00 Copy Cannot Execute Since Host Cannot Disconnect";
      case 0x002C0000:
        return "2c00 Command Sequence Error";
      case 0x002C0100:
        return "2c01 Too Many Windows Specified";
      case 0x002C0200:
        return "2c02 Invalid Combination Of Windows Specified";
      case 0x002C0300:
        return "2c03 Current Program Area Is Not Empty";
      case 0x002C0400:
        return "2c04 Current Program Area Is Empty";
      case 0x002C0500:
        return "2c05 Illegal Power Condition Request";
      case 0x002C0600:
        return "2c06 Persistent Prevent Conflict";
      case 0x002C0700:
        return "2c07 Previous Busy Status";
      case 0x002C0800:
        return "2c08 Previous Task Set Full Status";
      case 0x002C0900:
        return "2c09 Previous Reservation Conflict Status";
      case 0x002C0A00:
        return "2c0a Partition Or Collection Contains User Objects";
      case 0x002C0B00:
        return "2c0b Not Reserved";
      case 0x002C0C00:
        return "2c0c Orwrite Generation Does Not Match";
      case 0x002C0D00:
        return "2c0d Reset Write Pointer Not Allowed";
      case 0x002C0E00:
        return "2c0e Zone Is Offline";
      case 0x002C0F00:
        return "2c0f Stream Not Open";
      case 0x002C1000:
        return "2c10 Unwritten Data In Zone";
      case 0x002C1100:
        return "2c11 Descriptor Format Sense Data Required";
      case 0x002D0000:
        return "2d00 Overwrite Error On Update In Place";
      case 0x002E0000:
        return "2e00 Insufficient Time For Operation";
      case 0x002E0100:
        return "2e01 Command Timeout Before Processing";
      case 0x002E0200:
        return "2e02 Command Timeout During Processing";
      case 0x002E0300:
        return "2e03 Command Timeout During Processing Due To Error Recovery";
      case 0x002F0000:
        return "2f00 Commands Cleared By Another Initiator";
      case 0x002F0100:
        return "2f01 Commands Cleared By Power Loss Notification";
      case 0x002F0200:
        return "2f02 Commands Cleared By Device Server";
      case 0x002F0300:
        return "2f03 Some Commands Cleared By Queuing Layer Event";
      case 0x00300000:
        return "3000 Incompatible Medium Installed";
      case 0x00300100:
        return "3001 Cannot Read Medium - Unknown Format";
      case 0x00300200:
        return "3002 Cannot Read Medium - Incompatible Format";
      case 0x00300300:
        return "3003 Cleaning Cartridge Installed";
      case 0x00300400:
        return "3004 Cannot Write Medium - Unknown Format";
      case 0x00300500:
        return "3005 Cannot Write Medium - Incompatible Format";
      case 0x00300600:
        return "3006 Cannot Format Medium - Incompatible Medium";
      case 0x00300700:
        return "3007 Cleaning Failure";
      case 0x00300800:
        return "3008 Cannot Write - Application Code Mismatch";
      case 0x00300900:
        return "3009 Current Session Not Fixated For Append";
      case 0x00300A00:
        return "300a Cleaning Request Rejected";
      case 0x00300C00:
        return "300c Worm Medium - Overwrite Attempted";
      case 0x00300D00:
        return "300d Worm Medium - Integrity Check";
      case 0x00301000:
        return "3010 Medium Not Formatted";
      case 0x00301100:
        return "3011 Incompatible Volume Type";
      case 0x00301200:
        return "3012 Incompatible Volume Qualifier";
      case 0x00301300:
        return "3013 Cleaning Volume Expired";
      case 0x00310000:
        return "3100 Medium Format Corrupted";
      case 0x00310100:
        return "3101 Format Command Failed";
      case 0x00310200:
        return "3102 Zoned Formatting Failed Due To Spare Linking";
      case 0x00310300:
        return "3103 Sanitize Command Failed";
      case 0x00320000:
        return "3200 No Defect Spare Location Available";
      case 0x00320100:
        return "3201 Defect List Update Failure";
      case 0x00330000:
        return "3300 Tape Length Error";
      case 0x00340000:
        return "3400 Enclosure Failure";
      case 0x00350000:
        return "3500 Enclosure Services Failure";
      case 0x00350100:
        return "3501 Unsupported Enclosure Function";
      case 0x00350200:
        return "3502 Enclosure Services Unavailable";
      case 0x00350300:
        return "3503 Enclosure Services Transfer Failure";
      case 0x00350400:
        return "3504 Enclosure Services Transfer Refused";
      case 0x00350500:
        return "3505 Enclosure Services Checksum Error";
      case 0x00360000:
        return "3600 Ribbon, Ink, Or Toner Failure";
      case 0x00370000:
        return "3700 Rounded Parameter";
      case 0x00380000:
        return "3800 Event Status Notification";
      case 0x00380200:
        return "3802 Esn - Power Management Class Event";
      case 0x00380400:
        return "3804 Esn - Media Class Event";
      case 0x00380600:
        return "3806 Esn - Device Busy Class Event";
      case 0x00380700:
        return "3807 Thin Provisioning Soft Threshold Reached";
      case 0x00390000:
        return "3900 Saving Parameters Not Supported";
      case 0x003A0000:
        return "3a00 Medium Not Present";
      case 0x003A0100:
        return "3a01 Medium Not Present - Tray Closed";
      case 0x003A0200:
        return "3a02 Medium Not Present - Tray Open";
      case 0x003A0300:
        return "3a03 Medium Not Present - Loadable";
      case 0x003A0400:
        return "3a04 Medium Not Present - Medium Auxiliary Memory Accessible";
      case 0x003B0000:
        return "3b00 Sequential Positioning Error";
      case 0x003B0100:
        return "3b01 Tape Position Error At Beginning-Of-Medium";
      case 0x003B0200:
        return "3b02 Tape Position Error At End-Of-Medium";
      case 0x003B0300:
        return "3b03 Tape Or Electronic Vertical Forms Unit Not Ready";
      case 0x003B0400:
        return "3b04 Slew Failure";
      case 0x003B0500:
        return "3b05 Paper Jam";
      case 0x003B0600:
        return "3b06 Failed To Sense Top-Of-Form";
      case 0x003B0700:
        return "3b07 Failed To Sense Bottom-Of-Form";
      case 0x003B0800:
        return "3b08 Reposition Error";
      case 0x003B0900:
        return "3b09 Read Past End Of Medium";
      case 0x003B0A00:
        return "3b0a Read Past Beginning Of Medium";
      case 0x003B0B00:
        return "3b0b Position Past End Of Medium";
      case 0x003B0C00:
        return "3b0c Position Past Beginning Of Medium";
      case 0x003B0D00:
        return "3b0d Medium Destination Element Full";
      case 0x003B0E00:
        return "3b0e Medium Source Element Empty";
      case 0x003B0F00:
        return "3b0f End Of Medium Reached";
      case 0x003B1100:
        return "3b11 Medium Magazine Not Accessible";
      case 0x003B1200:
        return "3b12 Medium Magazine Removed";
      case 0x003B1300:
        return "3b13 Medium Magazine Inserted";
      case 0x003B1400:
        return "3b14 Medium Magazine Locked";
      case 0x003B1500:
        return "3b15 Medium Magazine Unlocked";
      case 0x003B1600:
        return "3b16 Mechanical Positioning Or Changer Error";
      case 0x003B1700:
        return "3b17 Read Past End Of User Object";
      case 0x003B1800:
        return "3b18 Element Disabled";
      case 0x003B1900:
        return "3b19 Element Enabled";
      case 0x003B1A00:
        return "3b1a Data Transfer Device Removed";
      case 0x003B1B00:
        return "3b1b Data Transfer Device Inserted";
      case 0x003B1C00:
        return "3b1c Too Many Logical Objects On Partition To Support Operation";
      case 0x003C0000:
        return "3c00";
      case 0x003D0000:
        return "3d00 Invalid Bits In Identify Message";
      case 0x003E0000:
        return "3e00 Logical Unit Has Not Self-Configured Yet";
      case 0x003E0100:
        return "3e01 Logical Unit Failure";
      case 0x003E0200:
        return "3e02 Timeout On Logical Unit";
      case 0x003E0300:
        return "3e03 Logical Unit Failed Self-Test";
      case 0x003E0400:
        return "3e04 Logical Unit Unable To Update Self-Test Log";
      case 0x003F0000:
        return "3f00 Target Operating Conditions Have Changed";
      case 0x003F0100:
        return "3f01 Microcode Has Been Changed";
      case 0x003F0200:
        return "3f02 Changed Operating Definition";
      case 0x003F0300:
        return "3f03 Inquiry Data Has Changed";
      case 0x003F0400:
        return "3f04 Component Device Attached";
      case 0x003F0500:
        return "3f05 Device Identifier Changed";
      case 0x003F0600:
        return "3f06 Redundancy Group Created Or Modified";
      case 0x003F0700:
        return "3f07 Redundancy Group Deleted";
      case 0x003F0800:
        return "3f08 Spare Created Or Modified";
      case 0x003F0900:
        return "3f09 Spare Deleted";
      case 0x003F0A00:
        return "3f0a Volume Set Created Or Modified";
      case 0x003F0B00:
        return "3f0b Volume Set Deleted";
      case 0x003F0C00:
        return "3f0c Volume Set Deassigned";
      case 0x003F0D00:
        return "3f0d Volume Set Reassigned";
      case 0x003F0E00:
        return "3f0e Reported Luns Data Has Changed";
      case 0x003F0F00:
        return "3f0f Echo Buffer Overwritten";
      case 0x003F1000:
        return "3f10 Medium Loadable";
      case 0x003F1100:
        return "3f11 Medium Auxiliary Memory Accessible";
      case 0x003F1200:
        return "3f12 Iscsi Ip Address Added";
      case 0x003F1300:
        return "3f13 Iscsi Ip Address Removed";
      case 0x003F1400:
        return "3f14 Iscsi Ip Address Changed";
      case 0x003F1500:
        return "3f15 Inspect Referrals Sense Descriptors";
      case 0x003F1600:
        return "3f16 Microcode Has Been Changed Without Reset";
      case 0x003F1700:
        return "3f17 Zone Transition To Full";
      case 0x003F1800:
        return "3f18 Bind Completed";
      case 0x003F1900:
        return "3f19 Bind Redirected";
      case 0x003F1A00:
        return "3f1a Subsidiary Binding Changed";
      case 0x00400000:
        return "4000 Ram Failure (Should Use 40 Nn)";
      // case 0x0040NN00:
      //   return "40nn Diagnostic Failure On Component Nn (80h-Ffh)";
      case 0x00410000:
        return "4100 Data Path Failure (Should Use 40 Nn)";
      case 0x00420000:
        return "4200 Power-On Or Self-Test Failure (Should Use 40 Nn)";
      case 0x00430000:
        return "4300 Message Error";
      case 0x00440000:
        return "4400 Internal Target Failure";
      case 0x00440100:
        return "4401 Persistent Reservation Information Lost";
      case 0x00447100:
        return "4471 Ata Device Failed Set Features";
      case 0x00450000:
        return "4500 Select Or Reselect Failure";
      case 0x00460000:
        return "4600 Unsuccessful Soft Reset";
      case 0x00470000:
        return "4700 Scsi Parity Error";
      case 0x00470100:
        return "4701 Data Phase Crc Error Detected";
      case 0x00470200:
        return "4702 Scsi Parity Error Detected During St Data Phase";
      case 0x00470300:
        return "4703 Information Unit Iucrc Error Detected";
      case 0x00470400:
        return "4704 Asynchronous Information Protection Error Detected";
      case 0x00470500:
        return "4705 Protocol Service Crc Error";
      case 0x00470600:
        return "4706 Phy Test Function In Progress";
      case 0x00477F00:
        return "477f Some Commands Cleared By Iscsi Protocol Event";
      case 0x00480000:
        return "4800 Initiator Detected Error Message Received";
      case 0x00490000:
        return "4900 Invalid Message Error";
      case 0x004A0000:
        return "4a00 Command Phase Error";
      case 0x004B0000:
        return "4b00 Data Phase Error";
      case 0x004B0100:
        return "4b01 Invalid Target Port Transfer Tag Received";
      case 0x004B0200:
        return "4b02 Too Much Write Data";
      case 0x004B0300:
        return "4b03 Ack/Nak Timeout";
      case 0x004B0400:
        return "4b04 Nak Received";
      case 0x004B0500:
        return "4b05 Data Offset Error";
      case 0x004B0600:
        return "4b06 Initiator Response Timeout";
      case 0x004B0700:
        return "4b07 Connection Lost";
      case 0x004B0800:
        return "4b08 Data-In Buffer Overflow - Data Buffer Size";
      case 0x004B0900:
        return "4b09 Data-In Buffer Overflow - Data Buffer Descriptor Area";
      case 0x004B0A00:
        return "4b0a Data-In Buffer Error";
      case 0x004B0B00:
        return "4b0b Data-Out Buffer Overflow - Data Buffer Size";
      case 0x004B0C00:
        return "4b0c Data-Out Buffer Overflow - Data Buffer Descriptor Area";
      case 0x004B0D00:
        return "4b0d Data-Out Buffer Error";
      case 0x004B0E00:
        return "4b0e Pcie Fabric Error";
      case 0x004B0F00:
        return "4b0f Pcie Completion Timeout";
      case 0x004B1000:
        return "4b10 Pcie Completer Abort";
      case 0x004B1100:
        return "4b11 Pcie Poisoned Tlp Received";
      case 0x004B1200:
        return "4b12 Pcie Ecrc Check Failed";
      case 0x004B1300:
        return "4b13 Pcie Unsupported Request";
      case 0x004B1400:
        return "4b14 Pcie Acs Violation";
      case 0x004B1500:
        return "4b15 Pcie Tlp Prefix Blocked";
      case 0x004C0000:
        return "4c00 Logical Unit Failed Self-Configuration";
      // case 0x004DNN00:
      //   return "4dnn Tagged Overlapped Commands (Nn = Task Tag)";
      case 0x004E0000:
        return "4e00 Overlapped Commands Attempted";
      case 0x004F0000:
        return "4f00";
      case 0x00500000:
        return "5000 Write Append Error";
      case 0x00500100:
        return "5001 Write Append Position Error";
      case 0x00500200:
        return "5002 Position Error Related To Timing";
      case 0x00510000:
        return "5100 Erase Failure";
      case 0x00510100:
        return "5101 Erase Failure - Incomplete Erase Operation Detected";
      case 0x00520000:
        return "5200 Cartridge Fault";
      case 0x00530000:
        return "5300 Media Load Or Eject Failed";
      case 0x00530100:
        return "5301 Unload Tape Failure";
      case 0x00530200:
        return "5302 Medium Removal Prevented";
      case 0x00530300:
        return "5303 Medium Removal Prevented By Data Transfer Element";
      case 0x00530400:
        return "5304 Medium Thread Or Unthread Failure";
      case 0x00530500:
        return "5305 Volume Identifier Invalid";
      case 0x00530600:
        return "5306 Volume Identifier Missing";
      case 0x00530700:
        return "5307 Duplicate Volume Identifier";
      case 0x00530800:
        return "5308 Element Status Unknown";
      case 0x00530900:
        return "5309 Data Transfer Device Error - Load Failed";
      case 0x00530A00:
        return "530a Data Transfer Device Error - Unload Failed";
      case 0x00530B00:
        return "530b Data Transfer Device Error - Unload Missing";
      case 0x00530C00:
        return "530c Data Transfer Device Error - Eject Failed";
      case 0x00530D00:
        return "530d Data Transfer Device Error - Library Communication Failed";
      case 0x00540000:
        return "5400 Scsi To Host System Interface Failure";
      case 0x00550000:
        return "5500 System Resource Failure";
      case 0x00550100:
        return "5501 System Buffer Full";
      case 0x00550200:
        return "5502 Insufficient Reservation Resources";
      case 0x00550300:
        return "5503 Insufficient Resources";
      case 0x00550400:
        return "5504 Insufficient Registration Resources";
      case 0x00550500:
        return "5505 Insufficient Access Control Resources";
      case 0x00550600:
        return "5506 Auxiliary Memory Out Of Space";
      case 0x00550700:
        return "5507 Quota Error";
      case 0x00550800:
        return "5508 Maximum Number Of Supplemental Decryption Keys Exceeded";
      case 0x00550900:
        return "5509 Medium Auxiliary Memory Not Accessible";
      case 0x00550A00:
        return "550a Data Currently Unavailable";
      case 0x00550B00:
        return "550b Insufficient Power For Operation";
      case 0x00550C00:
        return "550c Insufficient Resources To Create Rod";
      case 0x00550D00:
        return "550d Insufficient Resources To Create Rod Token";
      case 0x00550E00:
        return "550e Insufficient Zone Resources";
      case 0x00550F00:
        return "550f Insufficient Zone Resources To Complete Write";
      case 0x00551000:
        return "5510 Maximum Number Of Streams Open";
      case 0x00551100:
        return "5511 Insufficient Resources To Bind";
      case 0x00560000:
        return "5600";
      case 0x00570000:
        return "5700 Unable To Recover Table-Of-Contents";
      case 0x00580000:
        return "5800 Generation Does Not Exist";
      case 0x00590000:
        return "5900 Updated Block Read";
      case 0x005A0000:
        return "5a00 Operator Request Or State Change Input";
      case 0x005A0100:
        return "5a01 Operator Medium Removal Request";
      case 0x005A0200:
        return "5a02 Operator Selected Write Protect";
      case 0x005A0300:
        return "5a03 Operator Selected Write Permit";
      case 0x005B0000:
        return "5b00 Log Exception";
      case 0x005B0100:
        return "5b01 Threshold Condition Met";
      case 0x005B0200:
        return "5b02 Log Counter At Maximum";
      case 0x005B0300:
        return "5b03 Log List Codes Exhausted";
      case 0x005C0000:
        return "5c00 Rpl Status Change";
      case 0x005C0100:
        return "5c01 Spindles Synchronized";
      case 0x005C0200:
        return "5c02 Spindles Not Synchronized";
      case 0x005D0000:
        return "5d00 Failure Prediction Threshold Exceeded";
      case 0x005D0100:
        return "5d01 Media Failure Prediction Threshold Exceeded";
      case 0x005D0200:
        return "5d02 Logical Unit Failure Prediction Threshold Exceeded";
      case 0x005D0300:
        return "5d03 Spare Area Exhaustion Prediction Threshold Exceeded";
      case 0x005D1000:
        return "5d10 Hardware Impending Failure General Hard Drive Failure";
      case 0x005D1100:
        return "5d11 Hardware Impending Failure Drive Error Rate Too High";
      case 0x005D1200:
        return "5d12 Hardware Impending Failure Data Error Rate Too High";
      case 0x005D1300:
        return "5d13 Hardware Impending Failure Seek Error Rate Too High";
      case 0x005D1400:
        return "5d14 Hardware Impending Failure Too Many Block Reassigns";
      case 0x005D1500:
        return "5d15 Hardware Impending Failure Access Times Too High";
      case 0x005D1600:
        return "5d16 Hardware Impending Failure Start Unit Times Too High";
      case 0x005D1700:
        return "5d17 Hardware Impending Failure Channel Parametrics";
      case 0x005D1800:
        return "5d18 Hardware Impending Failure Controller Detected";
      case 0x005D1900:
        return "5d19 Hardware Impending Failure Throughput Performance";
      case 0x005D1A00:
        return "5d1a Hardware Impending Failure Seek Time Performance";
      case 0x005D1B00:
        return "5d1b Hardware Impending Failure Spin-Up Retry Count";
      case 0x005D1C00:
        return "5d1c Hardware Impending Failure Drive Calibration Retry Count";
      case 0x005D2000:
        return "5d20 Controller Impending Failure General Hard Drive Failure";
      case 0x005D2100:
        return "5d21 Controller Impending Failure Drive Error Rate Too High";
      case 0x005D2200:
        return "5d22 Controller Impending Failure Data Error Rate Too High";
      case 0x005D2300:
        return "5d23 Controller Impending Failure Seek Error Rate Too High";
      case 0x005D2400:
        return "5d24 Controller Impending Failure Too Many Block Reassigns";
      case 0x005D2500:
        return "5d25 Controller Impending Failure Access Times Too High";
      case 0x005D2600:
        return "5d26 Controller Impending Failure Start Unit Times Too High";
      case 0x005D2700:
        return "5d27 Controller Impending Failure Channel Parametrics";
      case 0x005D2800:
        return "5d28 Controller Impending Failure Controller Detected";
      case 0x005D2900:
        return "5d29 Controller Impending Failure Throughput Performance";
      case 0x005D2A00:
        return "5d2a Controller Impending Failure Seek Time Performance";
      case 0x005D2B00:
        return "5d2b Controller Impending Failure Spin-Up Retry Count";
      case 0x005D2C00:
        return "5d2c Controller Impending Failure Drive Calibration Retry Count";
      case 0x005D3000:
        return "5d30 Data Channel Impending Failure General Hard Drive Failure";
      case 0x005D3100:
        return "5d31 Data Channel Impending Failure Drive Error Rate Too High";
      case 0x005D3200:
        return "5d32 Data Channel Impending Failure Data Error Rate Too High";
      case 0x005D3300:
        return "5d33 Data Channel Impending Failure Seek Error Rate Too High";
      case 0x005D3400:
        return "5d34 Data Channel Impending Failure Too Many Block Reassigns";
      case 0x005D3500:
        return "5d35 Data Channel Impending Failure Access Times Too High";
      case 0x005D3600:
        return "5d36 Data Channel Impending Failure Start Unit Times Too High";
      case 0x005D3700:
        return "5d37 Data Channel Impending Failure Channel Parametrics";
      case 0x005D3800:
        return "5d38 Data Channel Impending Failure Controller Detected";
      case 0x005D3900:
        return "5d39 Data Channel Impending Failure Throughput Performance";
      case 0x005D3A00:
        return "5d3a Data Channel Impending Failure Seek Time Performance";
      case 0x005D3B00:
        return "5d3b Data Channel Impending Failure Spin-Up Retry Count";
      case 0x005D3C00:
        return "5d3c Data Channel Impending Failure Drive Calibration Retry Count";
      case 0x005D4000:
        return "5d40 Servo Impending Failure General Hard Drive Failure";
      case 0x005D4100:
        return "5d41 Servo Impending Failure Drive Error Rate Too High";
      case 0x005D4200:
        return "5d42 Servo Impending Failure Data Error Rate Too High";
      case 0x005D4300:
        return "5d43 Servo Impending Failure Seek Error Rate Too High";
      case 0x005D4400:
        return "5d44 Servo Impending Failure Too Many Block Reassigns";
      case 0x005D4500:
        return "5d45 Servo Impending Failure Access Times Too High";
      case 0x005D4600:
        return "5d46 Servo Impending Failure Start Unit Times Too High";
      case 0x005D4700:
        return "5d47 Servo Impending Failure Channel Parametrics";
      case 0x005D4800:
        return "5d48 Servo Impending Failure Controller Detected";
      case 0x005D4900:
        return "5d49 Servo Impending Failure Throughput Performance";
      case 0x005D4A00:
        return "5d4a Servo Impending Failure Seek Time Performance";
      case 0x005D4B00:
        return "5d4b Servo Impending Failure Spin-Up Retry Count";
      case 0x005D4C00:
        return "5d4c Servo Impending Failure Drive Calibration Retry Count";
      case 0x005D5000:
        return "5d50 Spindle Impending Failure General Hard Drive Failure";
      case 0x005D5100:
        return "5d51 Spindle Impending Failure Drive Error Rate Too High";
      case 0x005D5200:
        return "5d52 Spindle Impending Failure Data Error Rate Too High";
      case 0x005D5300:
        return "5d53 Spindle Impending Failure Seek Error Rate Too High";
      case 0x005D5400:
        return "5d54 Spindle Impending Failure Too Many Block Reassigns";
      case 0x005D5500:
        return "5d55 Spindle Impending Failure Access Times Too High";
      case 0x005D5600:
        return "5d56 Spindle Impending Failure Start Unit Times Too High";
      case 0x005D5700:
        return "5d57 Spindle Impending Failure Channel Parametrics";
      case 0x005D5800:
        return "5d58 Spindle Impending Failure Controller Detected";
      case 0x005D5900:
        return "5d59 Spindle Impending Failure Throughput Performance";
      case 0x005D5A00:
        return "5d5a Spindle Impending Failure Seek Time Performance";
      case 0x005D5B00:
        return "5d5b Spindle Impending Failure Spin-Up Retry Count";
      case 0x005D5C00:
        return "5d5c Spindle Impending Failure Drive Calibration Retry Count";
      case 0x005D6000:
        return "5d60 Firmware Impending Failure General Hard Drive Failure";
      case 0x005D6100:
        return "5d61 Firmware Impending Failure Drive Error Rate Too High";
      case 0x005D6200:
        return "5d62 Firmware Impending Failure Data Error Rate Too High";
      case 0x005D6300:
        return "5d63 Firmware Impending Failure Seek Error Rate Too High";
      case 0x005D6400:
        return "5d64 Firmware Impending Failure Too Many Block Reassigns";
      case 0x005D6500:
        return "5d65 Firmware Impending Failure Access Times Too High";
      case 0x005D6600:
        return "5d66 Firmware Impending Failure Start Unit Times Too High";
      case 0x005D6700:
        return "5d67 Firmware Impending Failure Channel Parametrics";
      case 0x005D6800:
        return "5d68 Firmware Impending Failure Controller Detected";
      case 0x005D6900:
        return "5d69 Firmware Impending Failure Throughput Performance";
      case 0x005D6A00:
        return "5d6a Firmware Impending Failure Seek Time Performance";
      case 0x005D6B00:
        return "5d6b Firmware Impending Failure Spin-Up Retry Count";
      case 0x005D6C00:
        return "5d6c Firmware Impending Failure Drive Calibration Retry Count";
      case 0x005DFF00:
        return "5dff Failure Prediction Threshold Exceeded (False)";
      case 0x005E0000:
        return "5e00 Low Power Condition On";
      case 0x005E0100:
        return "5e01 Idle Condition Activated By Timer";
      case 0x005E0200:
        return "5e02 Standby Condition Activated By Timer";
      case 0x005E0300:
        return "5e03 Idle Condition Activated By Command";
      case 0x005E0400:
        return "5e04 Standby Condition Activated By Command";
      case 0x005E0500:
        return "5e05 Idle_b Condition Activated By Timer";
      case 0x005E0600:
        return "5e06 Idle_b Condition Activated By Command";
      case 0x005E0700:
        return "5e07 Idle_c Condition Activated By Timer";
      case 0x005E0800:
        return "5e08 Idle_c Condition Activated By Command";
      case 0x005E0900:
        return "5e09 Standby_y Condition Activated By Timer";
      case 0x005E0A00:
        return "5e0a Standby_y Condition Activated By Command";
      case 0x005E4100:
        return "5e41 Power State Change To Active";
      case 0x005E4200:
        return "5e42 Power State Change To Idle";
      case 0x005E4300:
        return "5e43 Power State Change To Standby";
      case 0x005E4500:
        return "5e45 Power State Change To Sleep";
      case 0x005E4700:
        return "5e47 Power State Change To Device Control";
      case 0x005F0000:
        return "5f00";
      case 0x00600000:
        return "6000 Lamp Failure";
      case 0x00610000:
        return "6100 Video Acquisition Error";
      case 0x00610100:
        return "6101 Unable To Acquire Video";
      case 0x00610200:
        return "6102 Out Of Focus";
      case 0x00620000:
        return "6200 Scan Head Positioning Error";
      case 0x00630000:
        return "6300 End Of User Area Encountered On This Track";
      case 0x00630100:
        return "6301 Packet Does Not Fit In Available Space";
      case 0x00640000:
        return "6400 Illegal Mode For This Track";
      case 0x00640100:
        return "6401 Invalid Packet Size";
      case 0x00650000:
        return "6500 Voltage Fault";
      case 0x00660000:
        return "6600 Automatic Document Feeder Cover Up";
      case 0x00660100:
        return "6601 Automatic Document Feeder Lift Up";
      case 0x00660200:
        return "6602 Document Jam In Automatic Document Feeder";
      case 0x00660300:
        return "6603 Document Miss Feed Automatic In Document Feeder";
      case 0x00670000:
        return "6700 Configuration Failure";
      case 0x00670100:
        return "6701 Configuration Of Incapable Logical Units Failed";
      case 0x00670200:
        return "6702 Add Logical Unit Failed";
      case 0x00670300:
        return "6703 Modification Of Logical Unit Failed";
      case 0x00670400:
        return "6704 Exchange Of Logical Unit Failed";
      case 0x00670500:
        return "6705 Remove Of Logical Unit Failed";
      case 0x00670600:
        return "6706 Attachment Of Logical Unit Failed";
      case 0x00670700:
        return "6707 Creation Of Logical Unit Failed";
      case 0x00670800:
        return "6708 Assign Failure Occurred";
      case 0x00670900:
        return "6709 Multiply Assigned Logical Unit";
      case 0x00670A00:
        return "670a Set Target Port Groups Command Failed";
      case 0x00670B00:
        return "670b Ata Device Feature Not Enabled";
      case 0x00670C00:
        return "670c Command Rejected";
      case 0x00670D00:
        return "670d Explicit Bind Not Allowed";
      case 0x00680000:
        return "6800 Logical Unit Not Configured";
      case 0x00680100:
        return "6801 Subsidiary Logical Unit Not Configured";
      case 0x00690000:
        return "6900 Data Loss On Logical Unit";
      case 0x00690100:
        return "6901 Multiple Logical Unit Failures";
      case 0x00690200:
        return "6902 Parity/Data Mismatch";
      case 0x006A0000:
        return "6a00 Informational, Refer To Log";
      case 0x006B0000:
        return "6b00 State Change Has Occurred";
      case 0x006B0100:
        return "6b01 Redundancy Level Got Better";
      case 0x006B0200:
        return "6b02 Redundancy Level Got Worse";
      case 0x006C0000:
        return "6c00 Rebuild Failure Occurred";
      case 0x006D0000:
        return "6d00 Recalculate Failure Occurred";
      case 0x006E0000:
        return "6e00 Command To Logical Unit Failed";
      case 0x006F0000:
        return "6f00 Copy Protection Key Exchange Failure - Authentication Failure";
      case 0x006F0100:
        return "6f01 Copy Protection Key Exchange Failure - Key Not Present";
      case 0x006F0200:
        return "6f02 Copy Protection Key Exchange Failure - Key Not Established";
      case 0x006F0300:
        return "6f03 Read Of Scrambled Sector Without Authentication";
      case 0x006F0400:
        return "6f04 Media Region Code Is Mismatched To Logical Unit Region";
      case 0x006F0500:
        return "6f05 Drive Region Must Be Permanent/Region Reset Count Error";
      case 0x006F0600:
        return "6f06 Insufficient Block Count For Binding Nonce Recording";
      case 0x006F0700:
        return "6f07 Conflict In Binding Nonce Recording";
      case 0x006F0800:
        return "6f08 Insufficient Permission";
      case 0x006F0900:
        return "6f09 Invalid Drive-Host Pairing Server";
      case 0x006F0A00:
        return "6f0a Drive-Host Pairing Suspended";
      // case 0x0070NN00:
      //   return "70nn Decompression Exception Short Algorithm Id Of Nn";
      case 0x00710000:
        return "7100 Decompression Exception Long Algorithm Id";
      case 0x00720000:
        return "7200 Session Fixation Error";
      case 0x00720100:
        return "7201 Session Fixation Error Writing Lead-In";
      case 0x00720200:
        return "7202 Session Fixation Error Writing Lead-Out";
      case 0x00720300:
        return "7203 Session Fixation Error - Incomplete Track In Session";
      case 0x00720400:
        return "7204 Empty Or Partially Written Reserved Track";
      case 0x00720500:
        return "7205 No More Track Reservations Allowed";
      case 0x00720600:
        return "7206 Rmz Extension Is Not Allowed";
      case 0x00720700:
        return "7207 No More Test Zone Extensions Are Allowed";
      case 0x00730000:
        return "7300 Cd Control Error";
      case 0x00730100:
        return "7301 Power Calibration Area Almost Full";
      case 0x00730200:
        return "7302 Power Calibration Area Is Full";
      case 0x00730300:
        return "7303 Power Calibration Area Error";
      case 0x00730400:
        return "7304 Program Memory Area Update Failure";
      case 0x00730500:
        return "7305 Program Memory Area Is Full";
      case 0x00730600:
        return "7306 Rma/Pma Is Almost Full";
      case 0x00731000:
        return "7310 Current Power Calibration Area Almost Full";
      case 0x00731100:
        return "7311 Current Power Calibration Area Is Full";
      case 0x00731700:
        return "7317 Rdz Is Full";
      case 0x00740000:
        return "7400 Security Error";
      case 0x00740100:
        return "7401 Unable To Decrypt Data";
      case 0x00740200:
        return "7402 Unencrypted Data Encountered While Decrypting";
      case 0x00740300:
        return "7403 Incorrect Data Encryption Key";
      case 0x00740400:
        return "7404 Cryptographic Integrity Validation Failed";
      case 0x00740500:
        return "7405 Error Decrypting Data";
      case 0x00740600:
        return "7406 Unknown Signature Verification Key";
      case 0x00740700:
        return "7407 Encryption Parameters Not Useable";
      case 0x00740800:
        return "7408 Digital Signature Validation Failure";
      case 0x00740900:
        return "7409 Encryption Mode Mismatch On Read";
      case 0x00740A00:
        return "740a Encrypted Block Not Raw Read Enabled";
      case 0x00740B00:
        return "740b Incorrect Encryption Parameters";
      case 0x00740C00:
        return "740c Unable To Decrypt Parameter List";
      case 0x00740D00:
        return "740d Encryption Algorithm Disabled";
      case 0x00741000:
        return "7410 Sa Creation Parameter Value Invalid";
      case 0x00741100:
        return "7411 Sa Creation Parameter Value Rejected";
      case 0x00741200:
        return "7412 Invalid Sa Usage";
      case 0x00742100:
        return "7421 Data Encryption Configuration Prevented";
      case 0x00743000:
        return "7430 Sa Creation Parameter Not Supported";
      case 0x00744000:
        return "7440 Authentication Failed";
      case 0x00746100:
        return "7461 External Data Encryption Key Manager Access Error";
      case 0x00746200:
        return "7462 External Data Encryption Key Manager Error";
      case 0x00746300:
        return "7463 External Data Encryption Key Not Found";
      case 0x00746400:
        return "7464 External Data Encryption Request Not Authorized";
      case 0x00746E00:
        return "746e External Data Encryption Control Timeout";
      case 0x00746F00:
        return "746f External Data Encryption Control Error";
      case 0x00747100:
        return "7471 Logical Unit Access Not Authorized";
      case 0x00747900:
        return "7479 Security Conflict In Translated Device";
      case 0x00750000:
        return "7500";
      case 0x00760000:
        return "7600";
      case 0x00770000:
        return "7700";
      case 0x00780000:
        return "7800";
      case 0x00790000:
        return "7900";
      case 0x007A0000:
        return "7a00";
      case 0x007B0000:
        return "7b00";
      case 0x007C0000:
        return "7c00";
      case 0x007D0000:
        return "7d00";
      case 0x007E0000:
        return "7e00";
      case 0x007F0000:
        return "7f00";

      default:
        return NULL;
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
