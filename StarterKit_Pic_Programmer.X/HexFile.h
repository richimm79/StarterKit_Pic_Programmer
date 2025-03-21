#ifndef HEXFILE_H
#define	HEXFILE_H

extern UBYTE HexFile_RowLength;
extern UINT HexFile_Address;
extern UBYTE HexFile_RowType;
extern UBYTE HexFile_RowBuffer[0x200];

UBYTE HexFile_GetNibble();
BOOL HexFile_ExtractRow();
void HexFile_StartParser(STRING file, UINT charcount);

#endif	/* HEXFILE_H */

