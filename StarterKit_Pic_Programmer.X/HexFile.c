//PROTOCOL LOGIC
#include "main.h"

UBYTE HexFile_RowType;
UBYTE HexFile_RowLength;
UINT HexFile_Address;
UBYTE HexFile_CRC;

UINT HexFile_cursor;
UBYTE HexFile_RowBuffer[0x200];
STRING HexFile_file;
UINT HexFile_filecharcount;

UBYTE HexFile_ComputeCRC()
{
    UBYTE crc = HexFile_RowLength;
    crc += (HexFile_Address >> 24) & 0xff;
    crc += (HexFile_Address >> 16) & 0xff;
    crc += (HexFile_Address >> 8) & 0xff;
    crc += HexFile_Address & 0xff;
    crc += HexFile_RowType;
    for (UINT c = 0; c < HexFile_RowLength; c++)
    {
        crc += HexFile_RowBuffer[c];
    }
    return (~crc) + 1;
}

void HexFile_StartParser(STRING file, UINT charcount) 
{ 
    HexFile_cursor = 0; 
    HexFile_RowType = 0;
    HexFile_RowLength = 0;
    HexFile_Address = 0;
    HexFile_CRC = 0;
    
    HexFile_file = file;
    HexFile_filecharcount = charcount;
}

UBYTE HexFile_GetNibble()
{
    if (HexFile_cursor >= HexFile_filecharcount)
    {
        ErrorCode = eError_HexFileExpectedHexValue;
        return 0;
    }
    
    UBYTE val = HexFile_file[HexFile_cursor++];
    if (val >= '0' && val <= '9')
        return val - 48;
    if (val >= 'a' && val <= 'f')
        return val - 87;
    if (val >= 'A' && val <= 'F')
        return val - 55;

    ErrorCode = eError_HexFileExpectedHexValue;
    return 0;
}

USHORT HexFile_GetShort()
{
    UBYTE high = HexFile_GetNibble();
    UBYTE midh = HexFile_GetNibble();
    UBYTE mifl = HexFile_GetNibble();
    UBYTE low = HexFile_GetNibble();

    USHORT a1 = high << 12;
    USHORT a2 = midh << 8;
    USHORT a3 = mifl << 4;
    return a1 + a2 + a3 + low;
}

UBYTE HexFile_GetByte()
{
    UBYTE high = HexFile_GetNibble();
    UBYTE low = HexFile_GetNibble();
    
    UBYTE a1 = high << 4;
    return a1 + low;
}

void SkipSpaces()
{
    while (HexFile_cursor < HexFile_filecharcount)
    {
        UBYTE val = HexFile_file[HexFile_cursor];
        if (val == ':' || (val >= '0' && val <= '9') || (val >= 'A' && val <= 'F') || (val >= 'a' && val <= 'f'))
            break;
        HexFile_cursor++;
    }
}

BOOL HexFile_ExtractRow()
{
    SkipSpaces();
    if (HexFile_cursor >= HexFile_filecharcount)
    {
        ErrorCode = eError_HexFileEOF;
        return FALSE;
    }
    
    if (HexFile_file[HexFile_cursor++] != ':')
    {
        ErrorCode = eError_HexFileExpectedRowStart;
        return FALSE;
    }
    
    HexFile_RowLength = HexFile_GetByte();
    HexFile_Address = (HexFile_Address & 0xffff0000) + HexFile_GetShort();
    HexFile_RowType = HexFile_GetByte();
    
    if (ErrorCode != 0)
        return FALSE;

    for (UINT c = 0; c < HexFile_RowLength && ErrorCode == 0; c++)
    {
        HexFile_RowBuffer[c] = HexFile_GetByte();
    }
    HexFile_CRC = HexFile_GetByte();

    if (ErrorCode != 0)
        return FALSE;
    
    UBYTE crc = HexFile_ComputeCRC();
    if (crc == HexFile_CRC)
        return TRUE;
    
    ErrorCode = eError_HexFileCRCError;
    return FALSE;
}
