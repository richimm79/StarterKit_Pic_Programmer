#ifndef _ICSP_H
#define _ICSP_H

//Bit&Port assignment for the ICSP signals on PORTD
#define ICSPCLK (1)             //01-LED1 - RD0 - out - CLOCK
#define ICSPVPP (2)             //02-LED2 - RD1 - out - VPP
#define ICSPDATOUT (4)          //03-LED3 - RD2 - out - DATA Out
#define BUTTON_START (64)       //07-SW1  - RD6 - in  - Start cmd - active low
#define ICSPDATIN (512)         //10-     - RD9 - in  - DATA In
#define BUTTON_DOWNLOAD (8192)  //14-SW3  - RD13- in  - Download cmd - active low

#define SET_ICSPCLK (PORTDSET = ICSPCLK) 
#define CLR_ICSPCLK (PORTDCLR = ICSPCLK)

#define SET_ICSPDAT (PORTDCLR = ICSPDATOUT) //Negative logic for this pin
#define CLR_ICSPDAT (PORTDSET = ICSPDATOUT) //Negative logic for this pin

#define SET_ICSPVPP (PORTDSET = ICSPVPP)
#define CLR_ICSPVPP (PORTDCLR = ICSPVPP)

#define DATAINVALUE ((PORTD & ICSPDATIN) != 0)
#define START_PRESSED ((PORTD & BUTTON_START) == 0)
#define DOWNLOAD_PRESSES ((PORTD & BUTTON_DOWNLOAD) == 0)

//PIC10F22x time signals (nanoseconds)
#define SLOWDOWN *10
#define Tppdp (5000 SLOWDOWN)
#define Thld0 (5000 SLOWDOWN)
#define Tset1 (100 SLOWDOWN)
#define Thld1 (100 SLOWDOWN)
#define Tdly2 (1000 SLOWDOWN)
#define Tprog (1000000)
#define Tdis  (100000)
#define Tera  (10000000)

void ICSP();

BOOL Wait_Buttons();

void WriteData(UINT data);
USHORT ReadData();
BOOL WriteConfigWord(USHORT value);

#endif