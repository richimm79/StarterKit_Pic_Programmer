#ifndef _MAIN_H
#define _MAIN_H

#define UINT unsigned int
#define USHORT unsigned short
#define UBYTE unsigned char
#define UCHAR unsigned char
#define STRING unsigned char*
#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

#include <xc.h>
#include "PIC10F2XX.h"
#include "ICSP.h"
#include "HexFile.h"

enum 
{ 
    eError_PCOverrun = 1, 
    eError_HexFileAddressOutOfRange = 2, 
    eError_UnsupportedType = 3, 
    eError_HexFileEOF = 4, 
    eError_HexFileExpectedRowStart = 5, 
    eError_HexFileExpectedHexValue = 6,
    eError_HexFileCRCError = 7,
    eError_ReadDataNoMatch = 8,
    eError_CALVALUE_RestoreFailed = 9,
    eError_CALVALUE_Invalid = 10,
    eError_CALVALUE_ProgramOverwrite = 11,
    eError_ConfigWordError = 12
};
extern UINT ErrorCode;

#endif