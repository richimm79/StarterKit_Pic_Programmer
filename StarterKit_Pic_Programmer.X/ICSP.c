//PROTOCOL & HARDWARE DRIVER
//This file implements PIC10F222 hardware In-Circuit Serial Programming (ICSP)
//application for the "Ethernet Starter Kit" (PIC32MX7XX) DM320004 board
//Changes are required to support other devices.
//Instead of adding complexity to this file 
//duplicate it, one for each device, to have a driver implementation library.

//As a reference this code implements the flowcharts illustrated in the 
//Microchip document "Memory Programming Specification"

//NOTE: this file is the only one that includes xc.h, 
//there the only file hardware dependent.
//
//This file implements PIC10F222 hardware In-Circuit Serial Programming (ICSP)
//for the "Ethernet Starter Kit" (PIC32MX7XX) DM320004 board
//Changes are required to support other devices.
//Instead of adding complexity to this file duplicate it, one for each device.

//As a reference this code implements the flowcharts illustrated in the 
//Microchip document "Memory Programming Specification"

#include "main.h"

void ICSP()
{
    TRISDCLR = ICSPCLK + ICSPDATOUT + ICSPVPP; //RD0, RD1, RD2 as out pins
    CLR_ICSPCLK;
	SET_ICSPDAT;
    CLR_ICSPVPP;
}

BOOL Wait_Buttons()
{
    while (!START_PRESSED && !DOWNLOAD_PRESSES);
    return DOWNLOAD_PRESSES;
}

void BeginProgramming()
{
    ExecCommand(1,0,0,0); // xx1000 - no data
	DelayNanos(Tprog);
}

void EndProgramming()
{
    ExecCommand(1,1,1,0); // xx1110 - no data
	DelayNanos(Tdis);
}

//NOTE this won't work if memory value isn't 0xfff. Programming only writes '0', dose not writes '1'
void WriteData(UINT data)
{
    ExecCommand(0,0,1,0); //xx0010 - 16bit data
    
    SendBit(0); //Start bit
    SendBit((data & 0x0001) != 0);
    SendBit((data & 0x0002) != 0);
    SendBit((data & 0x0004) != 0);
    SendBit((data & 0x0008) != 0);
    SendBit((data & 0x0010) != 0);
    SendBit((data & 0x0020) != 0);
    SendBit((data & 0x0040) != 0);
    SendBit((data & 0x0080) != 0);
    SendBit((data & 0x0100) != 0);
    SendBit((data & 0x0200) != 0);
    SendBit((data & 0x0400) != 0);
    SendBit((data & 0x0800) != 0);
    SendBit((data & 0x1000) != 0);
    SendBit((data & 0x2000) != 0);
    SendBit(0); //Stop bit
	DelayNanos(Tdly2);

    BeginProgramming();
    EndProgramming();
}

USHORT ReadData()
{
    ExecCommand(0,1,0,0); // xx0100 - 16bit data
    
    SET_ICSPDAT;
    USHORT data = 0;
    ReceiveBit(); //Start bit
    if (ReceiveBit()) data += 0x0001;
    if (ReceiveBit()) data += 0x0002;
    if (ReceiveBit()) data += 0x0004;
    if (ReceiveBit()) data += 0x0008;
    if (ReceiveBit()) data += 0x0010;
    if (ReceiveBit()) data += 0x0020;
    if (ReceiveBit()) data += 0x0040;
    if (ReceiveBit()) data += 0x0080;
    if (ReceiveBit()) data += 0x0100;
    if (ReceiveBit()) data += 0x0200;
    if (ReceiveBit()) data += 0x0400;
    if (ReceiveBit()) data += 0x0800;
    if (ReceiveBit()) data += 0x1000;
    if (ReceiveBit()) data += 0x2000;
    ReceiveBit(); //Stop bit
    CLR_ICSPDAT;
    
	DelayNanos(Tdly2);
    IncrementAddress();
    return data;
}

BOOL WriteConfigWord(USHORT value)
{
    value = value | 0xFE0;
    //NOTE write ConfigWord is enabled only immediately after EnterProgramMode
    //--> No read and no moving PC before 

    EnterProgramMode();
    WriteData(value); 
    USHORT dataRead = ReadData();
    ExitProgramMode();

    if (dataRead != value)
    {
        ErrorCode = eError_ConfigWordError;
        return FALSE;
    }
    return TRUE;
}
