//BUSINESS LOGIC

//This program is written for the 
//     "Ethernet Starter Kit" (PIC32MX795F512L) DM320004 development board
//May work on other devices prior changes, see notes in PIC10xxxx.c

//A small circuit is required to connect the
//"Ethernet Starter Kit" to the PIC device, see README file.

//Copy your HEX file content in the HEX_file array below.
//Upload this application to the "Ethernet Starter Kit".
//Run the app.
//Press SW1 to start programming. 
//LED3 is on, and LED1 blinks, during programming time. 
//LED1 is steady on error.
//SW2 can be used to interrupt programming.
#include "main.h"


// DEVCFG3
#pragma config USERID = 0xFFFF          // Enter Hexadecimal value (Enter Hexadecimal value)
#pragma config FSRSSEL = PRIORITY_7     // SRS Select (SRS Priority 7)
#pragma config FMIIEN = ON              // Ethernet RMII/MII Enable (MII Enabled)
#pragma config FETHIO = ON              // Ethernet I/O Pin Select (Default Ethernet I/O)
#pragma config FCANIO = ON              // CAN I/O Pin Select (Default CAN I/O)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config UPLLIDIV = DIV_12        // USB PLL Input Divider (12x Divider)
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = ON             // Secondary Oscillator Enable (Enabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
#pragma config FCKSM = CSECME           // Clock Switching and Monitor Selection (Clock Switch Enable, FSCM Enabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = ON              // Watchdog Timer Enable (WDT Enabled)

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

static UCHAR HEX_file[] = 
":08000000010A6400C30BD00BE0"
":100386006700F80C0600C00C02000504070C2600E6"
":10039600030966000309C90B000A640C3C00000C43"
":1003A6003D00000C3E00000C3F001F022C001E0208"
":1003B6002B001D022A001C022900FF0CE901FF0C7C"
":1003C6000307EA01FF0C0307EB01FF0C0307EC012F"
":1003D600EC06FE0B0C024307FD0B0B024307FD0B5D"
":1003E6000A024307FD0B010C89004307FA0B0306BB"
":0803F600FD0BFE0BE00B0008FB"
":021FFE00EB0FE7"
":00000001FF"
;


//WORKING CODE
//static UCHAR HEX_file[] = 
//":08000000010A6400C30BD00BE0"
//":100386006700F80C0600C00C02000504070C2600E6"
//":10039600030966000309C90B000A100C3C00270C70"
//":1003A6003D00000C3E00000C3F001F022C001E0208"
//":1003B6002B001D022A001C022900FF0CE901FF0C7C"
//":1003C6000307EA01FF0C0307EB01FF0C0307EC012F"
//":1003D600EC06FE0B0C024307FD0B0B024307FD0B5D"
//":1003E6000A024307FD0B010C89004307FA0B0306BB"
//":0803F600FD0BFE0BE00B0008FB"
//":021FFE00EB0FE7"
//":00000001FF"
//;

UINT ErrorCode = 0;
BOOL CWAfterProgramming;
USHORT CWValue;

BOOL WriteRow()
{
    UBYTE n = HexFile_RowLength;
    for (UINT i = 0; i < n; i+=2)
    {
        UINT dataWrite = (HexFile_RowBuffer[i + 1] << 8) + HexFile_RowBuffer[i];
        WriteData(dataWrite);
        
        USHORT dataRead = ReadData();
        if (dataRead != dataWrite)
        {
            ErrorCode = eError_ReadDataNoMatch;
            return FALSE;
        }
	}
    return TRUE;
}

BOOL InterpretRow()
{
    if (HexFile_RowType == 0) //Data
    {
        if (HexFile_Address == 0x1ffe)
        {
            if (HexFile_RowLength != 2)
            {
                ErrorCode = eError_ConfigWordError;
                return FALSE;
            }
            CWAfterProgramming  = TRUE;
            CWValue = (HexFile_RowBuffer[1] << 8) + HexFile_RowBuffer[0];
            return TRUE;
        }
            
        MoveToAddress(HexFile_Address / 2); 
        return WriteRow();
    }
    
    if (HexFile_RowType == 1) //EndOfFile
        return FALSE;
    
    ErrorCode = eError_UnsupportedType; 
    return FALSE;
}

USHORT ReadBackupCALVALUE()
{
    EnterProgramMode();
    MoveToAddress(0x204);
    USHORT BackupOSCAL = ReadData();
    ExitProgramMode();
    return BackupOSCAL;
}

void Program()
{
    BulkErase();

    CWAfterProgramming = FALSE;
    
    EnterProgramMode();
    HexFile_StartParser(HEX_file, sizeof(HEX_file));
    while (HexFile_ExtractRow())
    {
        if (!InterpretRow())
            break;
    }
    ExitProgramMode();
    
    if (CWAfterProgramming && ErrorCode == 0)
        WriteConfigWord(CWValue);
}


BOOL CompareData()
{
    UBYTE n = HexFile_RowLength;
    for (UINT i = 0; i < n; i+=2)
    {
        UINT fileData = (HexFile_RowBuffer[i + 1] << 8) + HexFile_RowBuffer[i];
        USHORT dataRead = ReadData();
        if (fileData != dataRead)
        {
            ErrorCode = eError_ReadDataNoMatch;
            return FALSE;
        }
	}
    return TRUE;
}

BOOL CompareRow(USHORT CONFIGWORD)
{
    if (HexFile_RowType == 0) //Data
    {
        if (HexFile_Address == 0x1ffe) //Virtual address indicating Config Word location
        {
            if (HexFile_RowLength == 2)
                return CONFIGWORD == (HexFile_RowBuffer[1] << 8) + HexFile_RowBuffer[0];
            
            ErrorCode = eError_ConfigWordError;
            return FALSE;
        }
            
        MoveToAddress(HexFile_Address / 2);
        return CompareData();
    }
    
    if (HexFile_RowType == 1) //EndOfFile
        return FALSE;
    
    ErrorCode = eError_UnsupportedType; 
    return FALSE;
}

void Verify()
{
    EnterProgramMode();
    USHORT CONFIGWORD = ReadData();

    HexFile_StartParser(HEX_file, sizeof(HEX_file));
    while (HexFile_ExtractRow())
    {
        if (!CompareRow(CONFIGWORD))
            break;
    }
    
    if (ErrorCode != 0)
    {
        ExitProgramMode();
        return;
    }
    
    MoveToAddress(0x200);
    USHORT UserID1 = ReadData();
    USHORT UserID2 = ReadData();
    USHORT UserID3 = ReadData();
    USHORT UserID4 = ReadData();

    MoveToAddress(0x204);
    USHORT BackupOSCAL = ReadData();
    
    ExitProgramMode();
}
  
int main(int argc, char** argv) 
{   
    MCU_PIC10F2XX_Init();
    ICSP();
    
    //RestoreCALIBRATIONVALUE_From_Backup();    //Uncomment to restore CALIBRATION VALUE from backup location
    //FlashROM_Test();                          //Uncomment to wipe out and check program memory for write (DELETES  CALIBRATION VALUE)
    //WriteConfigWord(0xff8);                   //Uncomment to write you own config word

    while (1)
    {
        ErrorCode = 0;
        if(Wait_Buttons())
            Verify();
        else
            Program();

        ICSP_PIC10F2XX_Result(ErrorCode);
    }
    return 0;
}
