/* Arduino SdFat Library
 * Copyright (C) 2009 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SdFat_h
#define SdFat_h
/**
 * \file
 * SdFile and SdVolume classes
 */
#include <avr/pgmspace.h>
#include "zd2card.h"

uint8_t const BOOTSIG0 = 0X55;
uint8_t const BOOTSIG1 = 0XAA;

struct partitionTable {
          /**
           * Boot Indicator . Indicates whether the volume is the active
           * partition.  Legal values include: 0X00. Do not use for booting.
           * 0X80 Active partition.
           */
  uint8_t  boot;
          /**
            * Head part of Cylinder-head-sector address of the first block in
            * the partition. Legal values are 0-255. Only used in old PC BIOS.
            */
  uint8_t  beginHead;
          /**
           * Sector part of Cylinder-head-sector address of the first block in
           * the partition. Legal values are 1-63. Only used in old PC BIOS.
           */
  unsigned beginSector : 6;
           /** High bits cylinder for first block in partition. */
  unsigned beginCylinderHigh : 2;
          /**
           * Combine beginCylinderLow with beginCylinderHigh. Legal values
           * are 0-1023.  Only used in old PC BIOS.
           */
  uint8_t  beginCylinderLow;
          /**
           * Partition type. See defines that begin with PART_TYPE_ for
           * some Microsoft partition types.
           */
  uint8_t  type;
          /**
           * head part of cylinder-head-sector address of the last sector in the
           * partition.  Legal values are 0-255. Only used in old PC BIOS.
           */
  uint8_t  endHead;
          /**
           * Sector part of cylinder-head-sector address of the last sector in
           * the partition.  Legal values are 1-63. Only used in old PC BIOS.
           */
  unsigned endSector : 6;
           /** High bits of end cylinder */
  unsigned endCylinderHigh : 2;
          /**
           * Combine endCylinderLow with endCylinderHigh. Legal values
           * are 0-1023.  Only used in old PC BIOS.
           */
  uint8_t  endCylinderLow;
           /** Logical block address of the first block in the partition. */
  uint32_t firstSector;
           /** Length of the partition, in blocks. */
  uint32_t totalSectors;
};

typedef struct partitionTable part_t;
//------------------------------------------------------------------------------
/**
 * \struct masterBootRecord
 *
 * \brief Master Boot Record
 *
 * The first block of a storage device that is formatted with a MBR.
 */
struct masterBootRecord {
           /** Code Area for master boot program. */
  uint8_t  codeArea[440];
           /** Optional WindowsNT disk signature. May contain more boot code. */
  uint32_t diskSignature;
           /** Usually zero but may be more boot code. */
  uint16_t usuallyZero;
           /** Partition tables. */
  part_t   part[4];
           /** First MBR signature byte. Must be 0X55 */
  uint8_t  mbrSig0;
           /** Second MBR signature byte. Must be 0XAA */
  uint8_t  mbrSig1;
};
/** Type name for masterBootRecord */
typedef struct masterBootRecord mbr_t;

/** 
 * \struct biosParmBlock
 *
 * \brief BIOS parameter block
 * 
 *  The BIOS parameter block describes the physical layout of a FAT volume.
 */
struct biosParmBlock {
          /**
           * Count of bytes per sector. This value may take on only the
           * following values: 512, 1024, 2048 or 4096
           */
  uint16_t bytesPerSector;
          /**
           * Number of sectors per allocation unit. This value must be a
           * power of 2 that is greater than 0. The legal values are
           * 1, 2, 4, 8, 16, 32, 64, and 128.
           */
  uint8_t  sectorsPerCluster;
          /**
           * Number of sectors before the first FAT.
           * This value must not be zero.
           */
  uint16_t reservedSectorCount;
          /** The count of FAT data structures on the volume. This field should
           *  always contain the value 2 for any FAT volume of any type.
           */
  uint8_t  fatCount;
          /**
          * For FAT12 and FAT16 volumes, this field contains the count of
          * 32-byte directory entries in the root directory. For FAT32 volumes,
          * this field must be set to 0. For FAT12 and FAT16 volumes, this
          * value should always specify a count that when multiplied by 32
          * results in a multiple of bytesPerSector.  FAT16 volumes should
          * use the value 512.
          */
  uint16_t rootDirEntryCount;
          /**
           * This field is the old 16-bit total count of sectors on the volume.
           * This count includes the count of all sectors in all four regions
           * of the volume. This field can be 0; if it is 0, then totalSectors32
           * must be non-zero.  For FAT32 volumes, this field must be 0. For
           * FAT12 and FAT16 volumes, this field contains the sector count, and
           * totalSectors32 is 0 if the total sector count fits
           * (is less than 0x10000).
           */
  uint16_t totalSectors16;
          /**
           * This dates back to the old MS-DOS 1.x media determination and is
           * no longer usually used for anything.  0xF8 is the standard value
           * for fixed (non-removable) media. For removable media, 0xF0 is
           * frequently used. Legal values are 0xF0 or 0xF8-0xFF.
           */
  uint8_t  mediaType;
          /**
           * Count of sectors occupied by one FAT on FAT12/FAT16 volumes.
           * On FAT32 volumes this field must be 0, and sectorsPerFat32
           * contains the FAT size count.
           */
  uint16_t sectorsPerFat16;
           /** Sectors per track for interrupt 0x13. Not used otherwise. */
  uint16_t sectorsPerTrtack;
           /** Number of heads for interrupt 0x13.  Not used otherwise. */
  uint16_t headCount;
          /**
           * Count of hidden sectors preceding the partition that contains this
           * FAT volume. This field is generally only relevant for media
           *  visible on interrupt 0x13.
           */
  uint32_t hidddenSectors;
          /**
           * This field is the new 32-bit total count of sectors on the volume.
           * This count includes the count of all sectors in all four regions
           * of the volume.  This field can be 0; if it is 0, then
           * totalSectors16 must be non-zero.
           */
  uint32_t totalSectors32;
          /**
           * Count of sectors occupied by one FAT on FAT32 volumes.
           */
  uint32_t sectorsPerFat32;
          /**
           * This field is only defined for FAT32 media and does not exist on
           * FAT12 and FAT16 media.
           * Bits 0-3 -- Zero-based number of active FAT.
           *             Only valid if mirroring is disabled.
           * Bits 4-6 -- Reserved.
           * Bit 7  -- 0 means the FAT is mirrored at runtime into all FATs.
             *        -- 1 means only one FAT is active; it is the one referenced in bits 0-3.
           * Bits 8-15  -- Reserved.
           */
  uint16_t fat32Flags;
          /**
           * FAT32 version. High byte is major revision number.
           * Low byte is minor revision number. Only 0.0 define.
           */
  uint16_t fat32Version;
          /**
           * Cluster number of the first cluster of the root directory for FAT32.
           * This usually 2 but not required to be 2.
           */
  uint32_t fat32RootCluster;
          /**
           * Sector number of FSINFO structure in the reserved area of the
           * FAT32 volume. Usually 1.
           */
  uint16_t fat32FSInfo;
          /**
           * If non-zero, indicates the sector number in the reserved area
           * of the volume of a copy of the boot record. Usually 6.
           * No value other than 6 is recommended.
           */
  uint16_t fat32BackBootBlock;
          /**
           * Reserved for future expansion. Code that formats FAT32 volumes
           * should always set all of the bytes of this field to 0.
           */
  uint8_t  fat32Reserved[12];
};

/** Type name for biosParmBlock */
typedef struct biosParmBlock bpb_t;
//------------------------------------------------------------------------------
/**
 * \struct fat32BootSector
 *
 * \brief Boot sector for a FAT16 or FAT32 volume.
 * 
 */
struct fat32BootSector {
           /** X86 jmp to boot program */
  uint8_t  jmpToBootCode[3];
           /** informational only - don't depend on it */
  char     oemName[8];
           /** BIOS Parameter Block */
  bpb_t    bpb;
           /** for int0x13 use value 0X80 for hard drive */
  uint8_t  driveNumber;
           /** used by Windows NT - should be zero for FAT */
  uint8_t  reserved1;
           /** 0X29 if next three fields are valid */
  uint8_t  bootSignature;
           /** usually generated by combining date and time */
  uint32_t volumeSerialNumber;
           /** should match volume label in root dir */
  char     volumeLabel[11];
           /** informational only - don't depend on it */
  char     fileSystemType[8];
           /** X86 boot code */
  uint8_t  bootCode[420];
           /** must be 0X55 */
  uint8_t  bootSectorSig0;
           /** must be 0XAA */
  uint8_t  bootSectorSig1;
};
//------------------------------------------------------------------------------
// End Of Chain values for FAT entries
/** FAT16 end of chain value used by Microsoft. */
uint16_t const FAT16EOC = 0XFFFF;
/** Minimum value for FAT16 EOC.  Use to test for EOC. */
uint16_t const FAT16EOC_MIN = 0XFFF8;
/** FAT32 end of chain value used by Microsoft. */
uint32_t const FAT32EOC = 0X0FFFFFFF;
/** Minimum value for FAT32 EOC.  Use to test for EOC. */
uint32_t const FAT32EOC_MIN = 0X0FFFFFF8;
/** Mask a for FAT32 entry. Entries are 28 bits. */
uint32_t const FAT32MASK = 0X0FFFFFFF;

/** Type name for fat32BootSector */
typedef struct fat32BootSector fbs_t;

/**
 * \struct directoryEntry
 * \brief FAT short directory entry
 *
 * Short means short 8.3 name, not the entry size.
 *  
 * Date Format. A FAT directory entry date stamp is a 16-bit field that is 
 * basically a date relative to the MS-DOS epoch of 01/01/1980. Here is the
 * format (bit 0 is the LSB of the 16-bit word, bit 15 is the MSB of the 
 * 16-bit word):
 *   
 * Bits 9-15: Count of years from 1980, valid value range 0-127 
 * inclusive (1980-2107).
 *   
 * Bits 5-8: Month of year, 1 = January, valid value range 1-12 inclusive.
 *
 * Bits 0-4: Day of month, valid value range 1-31 inclusive.
 *
 * Time Format. A FAT directory entry time stamp is a 16-bit field that has
 * a granularity of 2 seconds. Here is the format (bit 0 is the LSB of the 
 * 16-bit word, bit 15 is the MSB of the 16-bit word).
 *   
 * Bits 11-15: Hours, valid value range 0-23 inclusive.
 * 
 * Bits 5-10: Minutes, valid value range 0-59 inclusive.
 *      
 * Bits 0-4: 2-second count, valid value range 0-29 inclusive (0 - 58 seconds).
 *   
 * The valid time range is from Midnight 00:00:00 to 23:59:58.
 */

struct directoryEntry {
           /**
            * Short 8.3 name.
            * The first eight bytes contain the file name with blank fill.
            * The last three bytes contain the file extension with blank fill.
            */
  uint8_t  name[11];
          /** Entry attributes.
           *
           * The upper two bits of the attribute byte are reserved and should
           * always be set to 0 when a file is created and never modified or
           * looked at after that.  See defines that begin with DIR_ATT_.
           */
  uint8_t  attributes;
          /**
           * Reserved for use by Windows NT. Set value to 0 when a file is
           * created and never modify or look at it after that.
           */
  uint8_t  reservedNT;
          /**
           * The granularity of the seconds part of creationTime is 2 seconds
           * so this field is a count of tenths of a second and its valid
           * value range is 0-199 inclusive. (WHG note - seems to be hundredths)
           */
  uint8_t  creationTimeTenths;
           /** Time file was created. */
  uint16_t creationTime;
           /** Date file was created. */
  uint16_t creationDate;
          /**
           * Last access date. Note that there is no last access time, only
           * a date.  This is the date of last read or write. In the case of
           * a write, this should be set to the same date as lastWriteDate.
           */
  uint16_t lastAccessDate;
          /**
           * High word of this entry's first cluster number (always 0 for a
           * FAT12 or FAT16 volume).
           */
  uint16_t firstClusterHigh;
           /** Time of last write. File creation is considered a write. */
  uint16_t lastWriteTime;
           /** Date of last write. File creation is considered a write. */
  uint16_t lastWriteDate;
           /** Low word of this entry's first cluster number. */
  uint16_t firstClusterLow;
           /** 32-bit unsigned holding this file's size in bytes. */
  uint32_t fileSize;
};

//------------------------------------------------------------------------------
// Definitions for directory entries
//
/** Type name for directoryEntry */
typedef struct directoryEntry dir_t;
/** escape for name[0] = 0XE5 */
uint8_t const DIR_NAME_0XE5 = 0X05;
/** name[0] value for entry that is free after being "deleted" */
uint8_t const DIR_NAME_DELETED = 0XE5;
/** name[0] value for entry that is free and no allocated entries follow */
uint8_t const DIR_NAME_FREE = 0X00;
/** file is read-only */
uint8_t const DIR_ATT_READ_ONLY = 0X01;
/** File should hidden in directory listings */
uint8_t const DIR_ATT_HIDDEN = 0X02;
/** Entry is for a system file */
uint8_t const DIR_ATT_SYSTEM = 0X04;
/** Directory entry contains the volume label */
uint8_t const DIR_ATT_VOLUME_ID = 0X08;
/** Entry is for a directory */
uint8_t const DIR_ATT_DIRECTORY = 0X10;
/** Old DOS archive bit for backup support */
uint8_t const DIR_ATT_ARCHIVE = 0X20;
/** Test value for long name entry.  Test is
  (d->attributes & DIR_ATT_LONG_NAME_MASK) == DIR_ATT_LONG_NAME. */
uint8_t const DIR_ATT_LONG_NAME = 0X0F;
/** Test mask for long name entry */
uint8_t const DIR_ATT_LONG_NAME_MASK = 0X3F;
/** defined attribute bits */
uint8_t const DIR_ATT_DEFINED_BITS = 0X3F;
/** Directory entry is part of a long name */
static inline uint8_t DIR_IS_LONG_NAME(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_LONG_NAME_MASK) == DIR_ATT_LONG_NAME;
}
/** Mask for file/subdirectory tests */
uint8_t const DIR_ATT_FILE_TYPE_MASK = (DIR_ATT_VOLUME_ID | DIR_ATT_DIRECTORY);
/** Directory entry is for a file */
static inline uint8_t DIR_IS_FILE(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_FILE_TYPE_MASK) == 0;
}
/** Directory entry is for a subdirectory */
static inline uint8_t DIR_IS_SUBDIR(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_FILE_TYPE_MASK) == DIR_ATT_DIRECTORY;
}
/** Directory entry is for a file or subdirectory */
static inline uint8_t DIR_IS_FILE_OR_SUBDIR(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_VOLUME_ID) == 0;
}

/**
 * Allow use of deprecated functions if non-zero
 */
#define ALLOW_DEPRECATED_FUNCTIONS 1
//------------------------------------------------------------------------------
// forward declaration since SdVolume is used in SdFile
class SdVolume;
//==============================================================================
// SdFile class

// flags for ls()
/** ls() flag to print modify date */
uint8_t const LS_DATE = 1;
/** ls() flag to print file size */
uint8_t const LS_SIZE = 2;
/** ls() flag for recursive list of subdirectories */
uint8_t const LS_R = 4;

// use the gnu style oflag in open()
/** open() oflag for reading */
uint8_t const O_READ = 0X01;
/** open() oflag - same as O_READ */
uint8_t const O_RDONLY = O_READ;
/** open() oflag for write */
uint8_t const O_WRITE = 0X02;
/** open() oflag - same as O_WRITE */
uint8_t const O_WRONLY = O_WRITE;
/** open() oflag for reading and writing */
uint8_t const O_RDWR = (O_READ | O_WRITE);
/** open() oflag mask for access modes */
uint8_t const O_ACCMODE = (O_READ | O_WRITE);
/** The file offset shall be set to the end of the file prior to each write. */
uint8_t const O_APPEND = 0X04;
/** synchronous writes - call sync() after each write */
uint8_t const O_SYNC = 0X08;
/** create the file if nonexistent */
uint8_t const O_CREAT = 0X10;
/** If O_CREAT and O_EXCL are set, open() shall fail if the file exists */
uint8_t const O_EXCL = 0X20;
/** truncate the file to zero length */
uint8_t const O_TRUNC = 0X40;

// flags for timestamp
/** set the file's last access date */
uint8_t const T_ACCESS = 1;
/** set the file's creation date and time */
uint8_t const T_CREATE = 2;
/** Set the file's write date and time */
uint8_t const T_WRITE = 4;
// values for type_
/** This SdFile has not been opened. */
uint8_t const FAT_FILE_TYPE_CLOSED = 0;
/** SdFile for a file */
uint8_t const FAT_FILE_TYPE_NORMAL = 1;
/** SdFile for a FAT16 root directory */
uint8_t const FAT_FILE_TYPE_ROOT16 = 2;
/** SdFile for a FAT32 root directory */
uint8_t const FAT_FILE_TYPE_ROOT32 = 3;
/** SdFile for a subdirectory */
uint8_t const FAT_FILE_TYPE_SUBDIR = 4;
/** Test value for directory type */
uint8_t const FAT_FILE_TYPE_MIN_DIR = FAT_FILE_TYPE_ROOT16;

/** date field for FAT directory entry */
static inline uint16_t FAT_DATE(uint16_t year, uint8_t month, uint8_t day) {
  return (year - 1980) << 9 | month << 5 | day;
}
/** year part of FAT directory date field */
static inline uint16_t FAT_YEAR(uint16_t fatDate) {
  return 1980 + (fatDate >> 9);
}
/** month part of FAT directory date field */
static inline uint8_t FAT_MONTH(uint16_t fatDate) {
  return (fatDate >> 5) & 0XF;
}
/** day part of FAT directory date field */
static inline uint8_t FAT_DAY(uint16_t fatDate) {
  return fatDate & 0X1F;
}
/** time field for FAT directory entry */
static inline uint16_t FAT_TIME(uint8_t hour, uint8_t minute, uint8_t second) {
  return hour << 11 | minute << 5 | second >> 1;
}
/** hour part of FAT directory time field */
static inline uint8_t FAT_HOUR(uint16_t fatTime) {
  return fatTime >> 11;
}
/** minute part of FAT directory time field */
static inline uint8_t FAT_MINUTE(uint16_t fatTime) {
  return(fatTime >> 5) & 0X3F;
}
/** second part of FAT directory time field */
static inline uint8_t FAT_SECOND(uint16_t fatTime) {
  return 2*(fatTime & 0X1F);
}
/** Default date for file timestamps is 1 Jan 2000 */
uint16_t const FAT_DEFAULT_DATE = ((2000 - 1980) << 9) | (1 << 5) | 1;
/** Default time for file timestamp is 1 am */
uint16_t const FAT_DEFAULT_TIME = (1 << 11);
//------------------------------------------------------------------------------
/**
 * \class SdFile
 * \brief Access FAT16 and FAT32 files on SD and SDHC cards.
 */
class SdFile //: public Print
{
public:
    SdFile(void) : type_(FAT_FILE_TYPE_CLOSED) {}
    void clearUnbufferedRead() { flags_ &= ~F_FILE_UNBUFFERED_READ; }
    uint8_t close();
    uint8_t contiguousRange(uint32_t* bgnBlock, uint32_t* endBlock);
    uint8_t createContiguous(SdFile* dirFile, const char* fileName, uint32_t size);
    uint32_t curCluster(void) const {return curCluster_;}
    uint32_t curPosition(void) const {return curPosition_;}
  /**
   * Set the date/time callback function
   *
   * \param[in] dateTime The user's call back function.  The callback
   * function is of the form:
   *
   * \code
   * void dateTime(uint16_t* date, uint16_t* time) {
   *   uint16_t year;
   *   uint8_t month, day, hour, minute, second;
   *
   *   // User gets date and time from GPS or real-time clock here
   *
   *   // return date using FAT_DATE macro to format fields
   *   *date = FAT_DATE(year, month, day);
   *
   *   // return time using FAT_TIME macro to format fields
   *   *time = FAT_TIME(hour, minute, second);
   * }
   * \endcode
   *
   * Sets the function that is called when a file is created or when
   * a file's directory entry is modified by sync(). All timestamps,
   * access, creation, and modify, are set when a file is created.
   * sync() maintains the last access date and last modify date/time.
   *
   * See the timestamp() function.
   */
    static void dateTimeCallback(
        void (*dateTime)(uint16_t* date, uint16_t* time)) {
        dateTime_ = dateTime;
    }
    /**
    * Cancel the date/time callback function.
    */
    static void dateTimeCallbackCancel() { dateTime_ = 0; }
    /** \return Address of the block that contains this file's directory. */
    uint32_t dirBlock(void) const {return dirBlock_;}
    uint8_t dirEntry(dir_t* dir);
    /** \return Index of this file's directory in the block dirBlock. */
    uint8_t dirIndex(void) const {return dirIndex_;}
    static void dirName(const dir_t& dir, char* name);
    /** \return The total number of bytes in a file or directory. */
    uint32_t fileSize(void) const {return fileSize_;}
    /** \return The first cluster number for a file or directory. */
    uint32_t firstCluster(void) const {return firstCluster_;}
    /** \return True if this is a SdFile for a directory else false. */
    uint8_t isDir(void) const {return type_ >= FAT_FILE_TYPE_MIN_DIR;}
    /** \return True if this is a SdFile for a file else false. */
    uint8_t isFile(void) const {return type_ == FAT_FILE_TYPE_NORMAL;}
    /** \return True if this is a SdFile for an open file/directory else false. */
    uint8_t isOpen(void) const {return type_ != FAT_FILE_TYPE_CLOSED;}
    /** \return True if this is a SdFile for a subdirectory else false. */
    uint8_t isSubDir(void) const {return type_ == FAT_FILE_TYPE_SUBDIR;}
    /** \return True if this is a SdFile for the root directory. */
    uint8_t isRoot() const {
        return type_ == FAT_FILE_TYPE_ROOT16 || type_ == FAT_FILE_TYPE_ROOT32;
    }

    void ls(uint8_t flags = 0, uint8_t indent = 0);
    uint8_t makeDir(SdFile* dir, const char* dirName);
    uint8_t open(SdFile* dirFile, uint16_t index, uint8_t oflag);
    uint8_t open(SdFile* dirFile, const char* fileName, uint8_t oflag);

    uint8_t openRoot(SdVolume* vol);
    static void printDirName(const dir_t& dir, uint8_t width);
    static void printFatDate(uint16_t fatDate);
    static void printFatTime(uint16_t fatTime);
    static void printTwoDigits(uint8_t v);
    /**
    * Read the next byte from a file.
    *
    * \return For success read returns the next byte in the file as an int.
    * If an error occurs or end of file is reached -1 is returned.
    */
    int16_t read() { uint8_t b; return read(&b, 1) == 1 ? b : -1; }
    int16_t read(void* buf, uint16_t nbyte);
    int8_t readDir(dir_t* dir);
    static uint8_t remove(SdFile* dirFile, const char* fileName);
    uint8_t remove(void);
    /** Set the file's current position to zero. */
    void rewind(void) { curPosition_ = curCluster_ = 0; }
    uint8_t rmDir(void);
    uint8_t rmRfStar(void);
    /** Set the files position to current position + \a pos. See seekSet(). */
    uint8_t seekCur(uint32_t pos) { return seekSet(curPosition_ + pos); }
  /**
   *  Set the files current position to end of file.  Useful to position
   *  a file for append. See seekSet().
   */
  uint8_t seekEnd(void) {return seekSet(fileSize_);}
  uint8_t seekSet(uint32_t pos);
  /**
   * Use unbuffered reads to access this file.  Used with Wave
   * Shield ISR.  Used with Sd2Card::partialBlockRead() in WaveRP.
   *
   * Not recommended for normal applications.
   */
    void setUnbufferedRead() { if (isFile()) flags_ |= F_FILE_UNBUFFERED_READ; }

  uint8_t timestamp(uint8_t flag, uint16_t year, uint8_t month, uint8_t day,
          uint8_t hour, uint8_t minute, uint8_t second);
  uint8_t sync(void);
  /** Type of this SdFile.  You should use isFile() or isDir() instead of type()
   * if possible.
   *
   * \return The file or directory type.
   */
    uint8_t type(void) const {return type_;}
    uint8_t truncate(uint32_t size);
    uint8_t unbufferedRead(void) const { return flags_ & F_FILE_UNBUFFERED_READ; }
    SdVolume* volume(void) const {return vol_;}
    size_t write(uint8_t b) { return write(&b, 1); }
    size_t write(const void* buf, uint16_t nbyte);
    size_t write(const char* str) { return write(str, strlen(str)); }
    void write_P(PGM_P str) { for (uint8_t c; (c = pgm_read_byte(str)); str++) write(c); }
    void writeln_P(PGM_P str) { write_P(str); }
//------------------------------------------------------------------------------
#if ALLOW_DEPRECATED_FUNCTIONS
// Deprecated functions  - suppress cpplint warnings with NOLINT comment
  /** \deprecated Use:
   * uint8_t SdFile::contiguousRange(uint32_t* bgnBlock, uint32_t* endBlock);
   */
  uint8_t contiguousRange(uint32_t& bgnBlock, uint32_t& endBlock) {  // NOLINT
    return contiguousRange(&bgnBlock, &endBlock);
  }
 /** \deprecated Use:
   * uint8_t SdFile::createContiguous(SdFile* dirFile,
   *   const char* fileName, uint32_t size)
   */
  uint8_t createContiguous(SdFile& dirFile,  // NOLINT
    const char* fileName, uint32_t size) {
    return createContiguous(&dirFile, fileName, size);
  }

  /**
   * \deprecated Use:
   * static void SdFile::dateTimeCallback(
   *   void (*dateTime)(uint16_t* date, uint16_t* time));
   */
  static void dateTimeCallback(
    void (*dateTime)(uint16_t& date, uint16_t& time)) {  // NOLINT
    oldDateTime_ = dateTime;
    dateTime_ = dateTime ? oldToNew : 0;
  }
  /** \deprecated Use: uint8_t SdFile::dirEntry(dir_t* dir); */
    uint8_t dirEntry(dir_t& dir) {return dirEntry(&dir);}  // NOLINT
    /** \deprecated Use:
    * uint8_t SdFile::makeDir(SdFile* dir, const char* dirName);
    */
    uint8_t makeDir(SdFile& dir, const char* dirName) { return makeDir(&dir, dirName); }

    uint8_t open(SdFile& dirFile, const char* fileName, uint8_t oflag) {
        return open(&dirFile, fileName, oflag);
    }

    uint8_t open(SdFile& dirFile, const char* fn) { return open(dirFile, fn, O_RDWR); }
    /** \deprecated Use:
    * uint8_t SdFile::open(SdFile* dirFile, uint16_t index, uint8_t oflag);
    */
    uint8_t open(SdFile& dirFile, uint16_t index, uint8_t oflag) {  // NOLINT
        return open(&dirFile, index, oflag);
    }
  /** \deprecated Use: uint8_t SdFile::openRoot(SdVolume* vol); */
  uint8_t openRoot(SdVolume& vol) {return openRoot(&vol);}  // NOLINT

  /** \deprecated Use: int8_t SdFile::readDir(dir_t* dir); */
  int8_t readDir(dir_t& dir) {return readDir(&dir);}  // NOLINT
  /** \deprecated Use:
   * static uint8_t SdFile::remove(SdFile* dirFile, const char* fileName);
   */
  static uint8_t remove(SdFile& dirFile, const char* fileName) {  // NOLINT
    return remove(&dirFile, fileName);
  }
//------------------------------------------------------------------------------
// rest are private
private:
    static void (*oldDateTime_)(uint16_t& date, uint16_t& time);  // NOLINT
    static void oldToNew(uint16_t* date, uint16_t* time) {
        uint16_t d, t;
        oldDateTime_(d, t);
        *date = d;
        *time = t;
    }
#endif  // ALLOW_DEPRECATED_FUNCTIONS
private:
    // bits defined in flags_
    // should be 0XF
    static uint8_t const F_OFLAG = (O_ACCMODE | O_APPEND | O_SYNC);
    // available bits
    static uint8_t const F_UNUSED = 0X30;
    // use unbuffered SD read
    static uint8_t const F_FILE_UNBUFFERED_READ = 0X40;
    // sync of directory entry required
    static uint8_t const F_FILE_DIR_DIRTY = 0X80;

// make sure F_OFLAG is ok
#if ((F_UNUSED | F_FILE_UNBUFFERED_READ | F_FILE_DIR_DIRTY) & F_OFLAG)
#error flags_ bits conflict
#endif  // flags_ bits

  // private data
  uint8_t   flags_;         // See above for definition of flags_ bits
  uint8_t   type_;          // type of file see above for values
  uint32_t  curCluster_;    // cluster for current file position
  uint32_t  curPosition_;   // current file position in bytes from beginning
  uint32_t  dirBlock_;      // SD block that contains directory entry for file
  uint8_t   dirIndex_;      // index of entry in dirBlock 0 <= dirIndex_ <= 0XF
  uint32_t  fileSize_;      // file size in bytes
  uint32_t  firstCluster_;  // first cluster of file
  SdVolume* vol_;           // volume where file is located

  // private functions
  uint8_t addCluster(void);
  uint8_t addDirCluster(void);
  dir_t* cacheDirEntry(uint8_t action);
  static void (*dateTime_)(uint16_t* date, uint16_t* time);
  static uint8_t make83Name(const char* str, uint8_t* name);
  uint8_t openCachedEntry(uint8_t cacheIndex, uint8_t oflags);
  dir_t* readDirCache(void);
};
//==============================================================================
// SdVolume class
/**
 * \brief Cache for an SD data block
 */
union cache_t
{
    uint8_t  data[512];
    uint16_t fat16[256];
    uint32_t fat32[128];
    dir_t    dir[16];
    mbr_t    mbr;
    fbs_t    fbs;
};
//------------------------------------------------------------------------------
/**
 * \class SdVolume
 * \brief Access FAT16 and FAT32 volumes on SD and SDHC cards.
 */
class SdVolume
{
public:
    /** Create an instance of SdVolume */
    SdVolume(void) :allocSearchStart_(2), fatType_(0) {}
    /** Clear the cache and returns a pointer to the cache.  Used by the WaveRP
    *  recorder to do raw write to the SD card.  Not for normal apps.
    */
    static uint8_t* cacheClear(void) {
        cacheFlush();
        cacheBlockNumber_ = 0XFFFFFFFF;
        return cacheBuffer_.data;
    }
  /**
   * Initialize a FAT volume.  Try partition one first then try super
   * floppy format.
   *
   * \param[in] dev The Sd2Card where the volume is located.
   *
   * \return The value one, true, is returned for success and
   * the value zero, false, is returned for failure.  Reasons for
   * failure include not finding a valid partition, not finding a valid
   * FAT file system or an I/O error.
   */
  uint8_t init(Sd2Card* dev) { return init(dev, 1) ? true : init(dev, 0);}
  uint8_t init(Sd2Card* dev, uint8_t part);

  // inline functions that return volume info
  /** \return The volume's cluster size in blocks. */
  uint8_t blocksPerCluster(void) const {return blocksPerCluster_;}
  /** \return The number of blocks in one FAT. */
  uint32_t blocksPerFat(void)  const {return blocksPerFat_;}
  /** \return The total number of clusters in the volume. */
  uint32_t clusterCount(void) const {return clusterCount_;}
  /** \return The shift count required to multiply by blocksPerCluster. */
  uint8_t clusterSizeShift(void) const {return clusterSizeShift_;}
  /** \return The logical block number for the start of file data. */
  uint32_t dataStartBlock(void) const {return dataStartBlock_;}
  /** \return The number of FAT structures on the volume. */
  uint8_t fatCount(void) const {return fatCount_;}
  /** \return The logical block number for the start of the first FAT. */
  uint32_t fatStartBlock(void) const {return fatStartBlock_;}
  /** \return The FAT type of the volume. Values are 12, 16 or 32. */
  uint8_t fatType(void) const {return fatType_;}
  /** \return The number of entries in the root directory for FAT16 volumes. */
  uint32_t rootDirEntryCount(void) const {return rootDirEntryCount_;}
  /** \return The logical block number for the start of the root directory
       on FAT16 volumes or the first cluster number on FAT32 volumes. */
  uint32_t rootDirStart(void) const {return rootDirStart_;}
  /** return a pointer to the Sd2Card object for this volume */
  static Sd2Card* sdCard(void) {return sdCard_;}
//------------------------------------------------------------------------------
#if ALLOW_DEPRECATED_FUNCTIONS
  // Deprecated functions  - suppress cpplint warnings with NOLINT comment
  /** \deprecated Use: uint8_t SdVolume::init(Sd2Card* dev); */
  uint8_t init(Sd2Card& dev) {return init(&dev);}  // NOLINT

    /** \deprecated Use: uint8_t SdVolume::init(Sd2Card* dev, uint8_t vol); */
    uint8_t init(Sd2Card& dev, uint8_t part) {  // NOLINT
        return init(&dev, part);
    }
#endif  // ALLOW_DEPRECATED_FUNCTIONS
private:
    // Allow SdFile access to SdVolume private data.
    friend class SdFile;

    // value for action argument in cacheRawBlock to indicate read from cache
    static uint8_t const CACHE_FOR_READ = 0;
    // value for action argument in cacheRawBlock to indicate cache dirty
    static uint8_t const CACHE_FOR_WRITE = 1;

    static cache_t cacheBuffer_;        // 512 byte cache for device blocks
    static uint32_t cacheBlockNumber_;  // Logical number of block in the cache
    static Sd2Card* sdCard_;            // Sd2Card object for cache
    static uint8_t cacheDirty_;         // cacheFlush() will write block if true
    static uint32_t cacheMirrorBlock_;  // block number for mirror FAT
    uint32_t allocSearchStart_;   // start cluster for alloc search
    uint8_t blocksPerCluster_;    // cluster size in blocks
    uint32_t blocksPerFat_;       // FAT size in blocks
    uint32_t clusterCount_;       // clusters in one FAT
    uint8_t clusterSizeShift_;    // shift to convert cluster count to block count
    uint32_t dataStartBlock_;     // first data block number
    uint8_t fatCount_;            // number of FATs on volume
    uint32_t fatStartBlock_;      // start block for first FAT
    uint8_t fatType_;             // volume type (12, 16, OR 32)
    uint16_t rootDirEntryCount_;  // number of entries in FAT16 root dir
    uint32_t rootDirStart_;       // root start block for FAT16, cluster for FAT32
    uint8_t allocContiguous(uint32_t count, uint32_t* curCluster);

    uint8_t blockOfCluster(uint32_t position) const {
        return position >> 9 & (blocksPerCluster_ - 1);}

    uint32_t clusterStartBlock(uint32_t cluster) const {
           return dataStartBlock_ + ((cluster - 2) << clusterSizeShift_);}

    uint32_t blockNumber(uint32_t cluster, uint32_t position) const {
           return clusterStartBlock(cluster) + blockOfCluster(position);}

    static uint8_t cacheFlush(void);
    static uint8_t cacheRawBlock(uint32_t blockNumber, uint8_t action);
    static void cacheSetDirty(void) {cacheDirty_ |= CACHE_FOR_WRITE;}
    static uint8_t cacheZeroBlock(uint32_t blockNumber);
    uint8_t chainSize(uint32_t beginCluster, uint32_t* size) const;
    uint8_t fatGet(uint32_t cluster, uint32_t* value) const;
    uint8_t fatPut(uint32_t cluster, uint32_t value);
    uint8_t fatPutEOC(uint32_t cluster) { return fatPut(cluster, 0x0FFFFFFF); }
    uint8_t freeChain(uint32_t cluster);

    uint8_t isEOC(uint32_t cluster) const {
        return cluster >= (fatType_ == 16 ? FAT16EOC_MIN : FAT32EOC_MIN);
    }

    uint8_t readBlock(uint32_t block, uint8_t* dst) { return sdCard_->readBlock(block, dst);}

    uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst) {
        return sdCard_->readData(block, offset, count, dst);
    }

    uint8_t writeBlock(uint32_t block, const uint8_t* dst) {
        return sdCard_->writeBlock(block, dst);
    }
};

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT)

class Fyle
{
private:
    char _name[13]; // our name
    SdFile *_file = 0;  // underlying file pointer
public:
    Fyle(SdFile f, const char *name);     // wraps an underlying SdFile
    Fyle() { _name[0] = 0; }
    ~Fyle() { }
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual size_t write(uint8_t val) { return write(&val, 1); }
    virtual int read() { return _file ? _file->read() : -1; }
    virtual int peek();
    virtual int available();
    virtual void flush();
    int read(void *buf, uint16_t nbyte) { return _file ? _file->read(buf, nbyte) : 0; }
    inline bool seek(uint32_t pos) { return _file ? _file->seekSet(pos) : false; } 
    uint32_t position() { return _file ? _file->curPosition() : -1; }
    uint32_t size() { return _file ? _file->fileSize() : 0; }
    void close();
    operator bool() { return _file ? _file->isOpen() : false; }
    char *name() { return _name; }
    bool isDirectory() { return _file && _file->isDir(); }
    Fyle openNextFile(uint8_t mode = O_RDONLY);
    void rewindDirectory(void);
};

class ZD
{
private:
    Port _portB;
    Pin _pin9;
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    SdFile getParentDir(const char *filepath, int *indx);

public:
    ZD() : _portB((uint8_t *)portb_base), _pin9(_portB, BIT1), card(&_pin9) { }
    ZD(Pin *cs) : _portB((uint8_t *)portb_base), _pin9(_portB, BIT1), card(cs) { }
    bool begin(uint8_t csPin = SD_CHIP_SELECT_PIN);
    Fyle open(const char *filename, uint8_t mode = FILE_READ);
    bool exists(char *filepath);
    bool mkdir(char *filepath);
    bool remove(char *filepath);
    bool rmdir(char *filepath);
private:
    int fileOpenMode;
    friend class Fyle;
    friend bool callback_openPath(SdFile&, char *, bool, void *);
};


#endif


