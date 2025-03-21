#include "main.h"

void FillEERom(USHORT value)
{
    MoveToAddress(0);
    for (UINT i = 0; i < 0x200 && ErrorCode == 0; i++)
    {
        WriteData(value);
        USHORT dataRead = ReadData();
        if (dataRead != value)
            ErrorCode = eError_ReadDataNoMatch;
    }
}

void FlashROM_Test()
{
    ErrorCode = 0;
    
    BulkErase();
    EnterProgramMode();
    FillEERom(0xaaa);
    ExitProgramMode();

    if (ErrorCode == 0)
    {
        BulkErase();
        EnterProgramMode();
        FillEERom(0x555);
        ExitProgramMode();
    }    
    
    ICSP_PIC10F2XX_Result(ErrorCode);
}

void RestoreCALIBRATIONVALUE_From_Backup()
{
    ErrorCode = 0;
    
    BulkErase();
    EnterProgramMode();
    
    MoveToAddress(0x204);
    USHORT BackupOSCAL = ReadData();
    ExitProgramMode();

    if ((BackupOSCAL & 0xff00) != 0xc00) //Validation. Must be 0xCnn
        ErrorCode = eError_CALVALUE_Invalid;
        
    else if (!Restore_CALVALUE(BackupOSCAL)) //Never skip this call
        ErrorCode = eError_CALVALUE_RestoreFailed;
    
    ICSP_PIC10F2XX_Result(ErrorCode);
}
