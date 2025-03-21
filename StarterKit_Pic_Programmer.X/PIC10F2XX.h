#ifndef _PIC10F2XX_H
#define _PIC10F2XX_H

extern UINT PC;

void MCU_PIC10F2XX_Init();
void ICSP_PIC10F2XX_Result(UINT errorCode);
void DelayNanos(UINT ns);

void EnterProgramMode();
void ExitProgramMode();

void SendBit(BOOL data);
BOOL ReceiveBit();
void IncrementAddress();
void ExecCommand(BOOL bit1, BOOL bit2, BOOL bit3, BOOL bit4);
void MoveToAddress(UINT value);
void BulkErase();

BOOL Restore_CALVALUE(USHORT value);
USHORT Read_CALVALUE();

#endif
