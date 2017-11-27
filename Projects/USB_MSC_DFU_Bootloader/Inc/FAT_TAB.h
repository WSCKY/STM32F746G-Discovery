#include "stm32f7xx_hal.h"

#define __LOBYTE(x)                    ((uint8_t)((x) & 0x00FF))
#define __HIBYTE(x)                    ((uint8_t)(((x) & 0xFF00) >>8))

#define FILE_DATE(y, m, d)             (((((uint16_t)(y) - 1980) & 0x7F) << 9) | \
                                                 (((uint16_t)(m) & 0x0F) << 5) | \
																			           (((uint16_t)(d) & 0x1F) << 0))

#define FILE_TIME(h, m, s)             ((((uint16_t)(h) & 0x1F) << 11) | \
                                         (((uint16_t)(m) & 0x3F) << 5) | \
																	       (((uint16_t)(s >> 1) & 0x1F) << 0))

/* File Attributes --------------------------------------------------------- */
#define FILE_ATTRIBUTES_RWE            0x00      /* Read Write */
#define FILE_ATTRIBUTES_RDO            0x01      /* Read only */
#define FILE_ATTRIBUTES_HID            0x02      /* Hidden */
#define	FILE_ATTRIBUTES_SYS	           0x04      /* System */
#define	FILE_ATTRIBUTES_VOL            0x08      /* Volume label */
#define FILE_ATTRIBUTES_LFN            0x0F      /* LFN entry */
#define FILE_ATTRIBUTES_DIR            0x10      /* Directory */
#define FILE_ATTRIBUTES_ARC            0x20      /* Archive */
#define FILE_ATTRIBUTES_MASK           0x3F      /* Mask of defined bits */

/* FATFS ERROR ------------------------------------------------------------- */
#define FATFS_OK                       (0)
#define FATFS_ERROR                    (-1)
/* FATFS DUMMY BYTE -------------------------------------------------------- */
#define RESERVED_BYTE                  ((uint8_t)0x00)

#define FAT_TABLE_ADDR						     0x8003800                /* 14K */
#define FLASH_PAGE_SIZE						     ((uint32_t)0x00000800)   /* FLASH Page Size */
/* SECTOR SIZE ------------------------------------------------------------- */
#define SECTOR_SIZE							       ((uint32_t)0x200)
#define SECTOR_IDX_TO_ADDR(idx)        ((idx) << 9)
#define SECTORS_CONV_BYTES(sec)        (SECTOR_IDX_TO_ADDR(sec))
#define BYTES_CONV_SECTORS(num)        ((num) >> 9)

#define FATFS_TOTAL_SECTORS            (9)
#define FATFS_USED_SIZE                (SECTORS_CONV_BYTES(FATFS_TOTAL_SECTORS))
#define FLASH_USER_SIZE                (200 * 1024)
#define FLASH_TOTAL_SECTORS            (BYTES_CONV_SECTORS(FLASH_USER_SIZE))

#define TOTAL_SECTORS                  (BYTES_CONV_SECTORS(FATFS_USED_SIZE + FLASH_USER_SIZE))

/* ---------- FATFS BOOT TABLE ---------- */
#define BOOT_TABLE_SIZE                SECTOR_SIZE
#define BOOT_TABLE_SECTOR_NUM          (1)
#define BOOT_TABLE_SECTOR_IDX          (0)
#define BOOT_TABLE_OFFSET              (0)
#define BOOT_TABLE_USED_SIZE           (62)
static uint8_t BOOT_TABLE[62] = { /* MSWIN4.1 */
0xEB, 0x3C, 0x90,  'k',  'y',  'C',  'h',  'u',  '^',  '_',  '^', __LOBYTE(SECTOR_SIZE), __HIBYTE(SECTOR_SIZE), 0x01, 0x01, 0x00, 
0x02, 0x20, 0x00, __LOBYTE(TOTAL_SECTORS), __HIBYTE(TOTAL_SECTORS), 0xF0, 0x03, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, RESERVED_BYTE, 0x29, 0x00, 0x00, 0x00, 0x00,  'k',  'y',  '.',  'C',  'h', 
 'u',  ' ',  'D',  'i',  's', 'k',  'F',  'A',  'T',  '1',  '2',  ' ',  ' ',  ' '
};

/* ---------- FATFS FATn TABLE ---------- */
#define FATn_TABLE_SIZE                (SECTOR_SIZE * 3)
#define FATn_TABLE_SECTOR_NUM          (3)
#define FAT1_TABLE_SECTOR_IDX          (1)
#define FAT1_TABLE_OFFSET              ((uint32_t)0x200)
#define FAT2_TABLE_SECTOR_IDX          (4)
#define FAT2_TABLE_OFFSET              ((uint32_t)0x800)
static uint8_t FATn_TABLE[FATn_TABLE_SIZE] = {0x00, 0x00, 0x00, 0x03, 0x40, 0x00, 0xFF, 0x0F};

/* ---------- FATFS ROOT TABLE ---------- */
#define ROOT_TABLE_SIZE                (SECTOR_SIZE * 2)
#define ROOT_TABLE_SECTOR_NUM          (2)
#define ROOT_TABLE_SECTOR_IDX          (7)
#define ROOT_TABLE_OFFSET              ((uint32_t)0xE00)
static uint8_t ROOT_TABLE[ROOT_TABLE_SIZE] = {
	/* TAB - 1 */
	 'k',  'y',  '.',  'C',  'h',  'u',  ' ',  'D',  'F',  'U',  ' ', FILE_ATTRIBUTES_VOL, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, 
	RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, __LOBYTE(FILE_TIME(22, 45, 32)), __HIBYTE(FILE_TIME(22, 45, 32)), __LOBYTE(FILE_DATE(1993, 8, 6)), __HIBYTE(FILE_DATE(1993, 8, 6)), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* TAB - 2 */
//	0xE5, 0xB0, 0x65, 0xFA, 0x5E, 0x87, 0x65, 0x2C, 0x67, 0x87, 0x65, FILE_ATTRIBUTES_LFN, 0x00, 0xD2, 0x63, 0x68, 
//	0x2E, 0x00, 0x74, 0x00, 0x78, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 
//0xE5, 0xC2, 0xBD, 0xA8, 0xCE, 0xC4, 0x7E, 0x31, 0x54, 0x58, 0x54, 0x20, 0x00, 0x00, 0xB0, 0xA6, 
//0x9B, 0x47, 0x9B, 0x47, 0x00, 0x00, 0xB1, 0xA6, 0x9B, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	/* TAB - 3 */
	 'R',  'E',  'A',  'D',  'M',  'E',  ' ',  ' ',  'T',  'X',  'T', FILE_ATTRIBUTES_RDO, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, 
	RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, RESERVED_BYTE, __LOBYTE(FILE_TIME(22, 45, 32)), __HIBYTE(FILE_TIME(22, 45, 32)), __LOBYTE(FILE_DATE(1993, 8, 6)), __HIBYTE(FILE_DATE(1993, 8, 6)), 0x02, 0x00, 0x14, 0x05, 0x00, 0x00,
};

#define README_FILE_LEN                1300
#define README_SECT_IDX                FATFS_TOTAL_SECTORS
#define README_SECT_NUM                (BYTES_CONV_SECTORS(README_FILE_LEN) + 1)

#define README_TAIL_LEN                (README_FILE_LEN - SECTORS_CONV_BYTES(BYTES_CONV_SECTORS(README_FILE_LEN)))
//const
static unsigned char README_DATA[README_FILE_LEN] =
"This is the first of a pair of papers on the theory and implementation of a\r\n"
"direction-cosine-matrix (DCM) based inertial measurement unit for application\r\n"
"in model planes and helicopters. Actually, at this point, it is still a draft, there\r\n"
"is still a lot more work to be done. Several reviewers, especially Louis\r\n"
"LeGrand and UFO-man, have made good suggestions on additions and\r\n"
"revisions that we should make and prepared some figures that we have not\r\n"
"included yet. We will eventually incorporate their suggestions, but it may take\r\n"
"a long time to get there. In the meantime, we think there is an audience who\r\n"
"can benefit from what we have so far.\r\n"
"The motivation for DCM was to take the next step in stabilization and\r\n"
"control functions from an inherently stable aircraft with elevator and rudder\r\n"
"control, to an aerobatic aircraft with ailerons and elevator. One of the authors\r\n"
"(Premerlani) built a two axes board several years ago, and developed\r\n"
"rudimentary firmware to provide stabilization and return-to-launch (RTL)\r\n"
"functions for a Gentle Lady sailplane. The firmware worked well enough, and\r\n"
"the author came to rely on the RTL feature, but it never seemed to work as\r\n"
"well as the author would like. In particular, satisfactory solutions to the\r\n"
"following two issues were never found:  ";
