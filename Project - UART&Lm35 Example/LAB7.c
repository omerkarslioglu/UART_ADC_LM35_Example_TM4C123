#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "TM4C123GH6PM.h"
#include <stdio.h>
void Delay(unsigned long counter);
void UART5_init(void);
void UART5_Transmitter(unsigned char data);
void printstring(char *str);
char mesg[12];
float voltage;
int main(void)
{
     unsigned int adc_value;
	   UART5_init();
    /* Enable Clock to ADC0 and GPIO pins*/
    SYSCTL->RCGCGPIO |= (1<<4);   /* Enable Clock to GPIOE or PE3/AN0 */
    SYSCTL->RCGCADC |= (1<<0);    /* AD0 clock enable*/
    
    /* initialize PE3 for AIN0 input  */
    GPIOE->AFSEL |= (1<<3);       /* enable alternate function */
    GPIOE->DEN &= ~(1<<3);        /* disable digital function */
    GPIOE->AMSEL |= (1<<3);       /* enable analog function */
   
    /* initialize sample sequencer3 */
    ADC0->ACTSS &= ~(1<<3);        /* disable SS3 during configuration */
    ADC0->EMUX &= ~0xF000;    /* software trigger conversion */
    ADC0->SSMUX3 = 0;         /* get input from channel 0 */
    ADC0->SSCTL3 |= (1<<1)|(1<<2);        /* take one sample at a time, set flag at 1st sample */
    ADC0->ACTSS |= (1<<3);         /* enable ADC0 sequencer 3 */
    
	
    while(1)
    {
        ADC0->PSSI |= (1<<3);        /* Enable SS3 conversion or start sampling data from AN0 */
        while((ADC0->RIS & 8) == 0) ;   /* Wait untill sample conversion completed*/
        adc_value = ADC0->SSFIFO3; /* read adc coversion result from SS3 FIFO*/
        ADC0->ISC = 8;          /* clear coversion clear flag bit*/
			  
			  /* convert digital value back into voltage */
			 voltage = (adc_value * 0.0008);
			sprintf(mesg, "\r\nVoltage = %0.3f Volts", voltage);
			printstring(mesg);
      Delay(2000);
    }
}

void UART5_init(void)
{
	SYSCTL->RCGCUART |= 0x20;  /* enable clock to UART5 */
    SYSCTL->RCGCGPIO |= 0x10;  /* enable clock to PORTE for PE4/Rx and RE5/Tx */
    Delay(1);
    /* UART0 initialization */
    UART5->CTL = 0;         /* UART5 module disbable */
    UART5->IBRD = 104;      /* for 9600 baud rate, integer = 104 */
    UART5->FBRD = 11;       /* for 9600 baud rate, fractional = 11*/
    UART5->CC = 0;          /*select system clock*/
    UART5->LCRH = 0x60;     /* data lenght 8-bit, not parity bit, no FIFO */
    UART5->CTL = 0x301;     /* Enable UART5 module, Rx and Tx */

    /* UART5 TX5 and RX5 use PE4 and PE5. Configure them digital and enable alternate function */
    GPIOE->DEN = 0x30;      /* set PE4 and PE5 as digital */
    GPIOE->AFSEL = 0x30;    /* Use PE4,PE5 alternate function */
    GPIOE->AMSEL = 0;    /* Turn off analg function*/
    GPIOE->PCTL = 0x00110000;     /* configure PE4 and PE5 for UART */
}
void UART5_Transmitter(unsigned char data)  
{
    while((UART5->FR & (1<<5)) != 0); /* wait until Tx buffer not full */
    UART5->DR = data;                  /* before giving it another byte */
}

void printstring(char *str)
{
  while(*str)
	{
		UART5_Transmitter(*(str++));
	}
}
void Delay(unsigned long counter)
{
	unsigned long i = 0;
	
	for(i=0; i< counter*1000; i++);
}
