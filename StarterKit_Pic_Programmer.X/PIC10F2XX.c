#include "main.h"

UINT PC; //Program Counter, this counts the 16 bit words

void MCU_PIC10F2XX_Init()
{
    //MCU init for max speed
    CHECON = 0x31;
    SYSKEY = 0; 
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
}

void ICSP_PIC10F2XX_Result(UINT errorCode)
{
    SET_ICSPDAT;
    if (errorCode != 0)
        SET_ICSPCLK;
    else
        CLR_ICSPDAT;
}

void DelayNanos(UINT ns)
{
    //NOTE: precision depends on MCU speed.
    ns /= 1; //adjust to get close to real timing
    _CP0_SET_COUNT(0);
    while (ns > _CP0_GET_COUNT());
}

void MoveToAddress(UINT value)
{
    while (PC != value)
        IncrementAddress();
}

void EnterProgramMode()
{
    PC = 0x3ff; //Initial Program Counter for PIC10F222
    CLR_ICSPCLK;
	CLR_ICSPDAT;
	DelayNanos(Tppdp);
	SET_ICSPVPP;

	DelayNanos(Thld0);
}

void ExitProgramMode()
{
    CLR_ICSPCLK;
	CLR_ICSPDAT;
    DelayNanos(Thld0);
	CLR_ICSPVPP;
}

BOOL ReceiveBit()
{
    SET_ICSPCLK;
	DelayNanos(Tset1);// asm("nop"); asm("nop");
    
    BOOL value = DATAINVALUE;
    
	CLR_ICSPCLK;
	DelayNanos(Thld1); // asm("nop"); asm("nop");

	return value;
}

void SendBit(BOOL data)
{
	SET_ICSPCLK;

	if (data)
		SET_ICSPDAT;
	else
		CLR_ICSPDAT;
	DelayNanos(Tset1); // asm("nop"); asm("nop");
	CLR_ICSPCLK;
	DelayNanos(Thld1); // asm("nop"); asm("nop");
}

void ExecCommand(BOOL bit1, BOOL bit2, BOOL bit3, BOOL bit4)
{
    SendBit(bit4);
    SendBit(bit3);
    SendBit(bit2);
    SendBit(bit1);
    SendBit(0); //x bit
    SendBit(0); //x bit
    DelayNanos(Tdly2);
}

void IncrementAddress()
{
    ExecCommand(0,1,1,0); // xx0110 - no data

    if ((PC & 0xffff) == 0x3ff)
        PC &= 0xffff0000; //Wrap around without incrementing MSB
    else
        PC++;
}

void BulkErase()
{
    EnterProgramMode();
    
    ExecCommand(1,0,0,1); // xx1001 - no data
	DelayNanos(Tera);
    
    ExitProgramMode();
}

USHORT Read_CALVALUE()
{
    EnterProgramMode();

    MoveToAddress(0x1ff);    
    USHORT CALVALUE = ReadData();
    
	ExitProgramMode();
    return CALVALUE;
}

BOOL Restore_CALVALUE(USHORT value)
{
    EnterProgramMode();
    
    MoveToAddress(0x1ff);
    WriteData(value);
    
    USHORT CALVALUE = ReadData();
	ExitProgramMode();
    if (CALVALUE == value)
        return TRUE;
    return FALSE;
}

