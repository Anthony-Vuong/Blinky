/*****************************************************************************
*   Author:     Anthony Vuong
*   Date:       6/17/2020
*   Project:    Blinky
*
******************************************************************************/

#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Micro-second scale
#define microScale 1000000

// Milli-second scale
#define milliScale 1000

//DCO Clock Frequencies
#define FREQ_1_5_MHz 15
#define FREQ_3_MHz 3
#define FREQ_6_MHz 6
#define FREQ_12_MHz 12
#define FREQ_24_MHz 24
#define FREQ_48_MHz 48
#define delayT 200
#define uSEC 1
#define mSEC 2

// function declarations
void WDT_IRQ();
int delay_us(signed int timeUnits, int frequency);
int delay_ms(signed int timeUnits, int frequency);
void NVIC_setup(void);
void chooseDCO(int freq);
void initTimer32(int count);
void delayTime(void);
int calcCount(int flag, int timeUnits, int frequency);
void setDCO(int frequency);



int main(void)
{
    //
    int count = 0,
        flag = 1,
        freq = FREQ_48_MHz;

    WDT_IRQ();
    NVIC_setup();
    setDCO(freq);

    count = calcCount(flag, 200, freq);
    initTimer32(count);

    //P1.0 set to output
    P1->DIR |= BIT0;

    delayTime(200);

    return 0;
}

void WDT_IRQ()
{
    //Turn off watch dog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // Enable global interrupt
    __enable_irq();

}

void delayTime(void)
{
    __delay_cycles(delayT);
}

int calcCount(int flag, int timeUnits, int frequency)
{
    int cycles = 0, count = 0;

    if(flag == uSEC)
    {
        cycles = frequency * 1000; // 1us
    }
    else
    {
        cycles = frequency * 1000000; // 1ms
    }

    count = timeUnits * cycles;  // x us

    return count;
}

void initTimer32(int count)
{
    // R R R R R R EN MODE IE  R  PRE  SIZE  WRAP
    // 0 0 0 0 0 0 1   1    1  0   00    1     1
    TIMER32_1->CONTROL = 0x000000E2;
    TIMER32_1->LOAD = count;
    TIMER32_1->INTCLR = 0;
}

void NVIC_setup(void)
{
    //Timer32_1 priority set & interrupt enabled
    NVIC->IP[25] |= 0x20;
    NVIC->ISER[0] |= 0x02000000;
}

void setDCO(int frequency)
{
    CS->KEY = CS_KEY_VAL;
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    chooseDCO(frequency);
    CS->KEY = 0;
}

void chooseDCO(int freq)
{
    if(freq == FREQ_1_5_MHz)
    {
        CS->CTL0 = CS_CTL0_DCORSEL_0;
    }
    else if(freq == FREQ_3_MHz)
    {
        CS->CTL0 = CS_CTL0_DCORSEL_1;
    }
    else if(freq == FREQ_6_MHz)
    {
        CS->CTL0 = CS_CTL0_DCORSEL_2;
    }
    else if(freq == FREQ_12_MHz)
    {
        CS->CTL0 = CS_CTL0_DCORSEL_3;
    }
   else if(freq == FREQ_24_MHz)
   {
       CS->CTL0 = CS_CTL0_DCORSEL_4;
   }
   else if(freq == FREQ_48_MHz)
   {
       CS->CTL0 = CS_CTL0_DCORSEL_5;
   }
   else
   {
       CS->CTL0 = CS_CTL0_DCORSEL_1;
   }
}



