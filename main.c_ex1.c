#include <intrinsics.h>
#include <msp430g2553.h>
#include <templateEMP.h>
// Name: Venkata Sai Harsha Rayaprolu
// Number: 4854097

/**
 * main.c
 */
int main(void)
{
    initMSP();              //Initialize micro-controller
	P1SEL=0x06;             // To initialize P1.4, P1.5 as GPIO and initialize the UART pins P1.1, P1.2
	P1SEL2=0x06;
	P1DIR |= BIT4;          //Set P1.4 as output
	P1DIR |= BIT5;          //Set P1.5 as output. This pin can also be used as a heater. It has to be configured
	                        // as a heater using P1SEL and P1SEL2
	P1OUT &= (BIT4 & BIT5); //Clear bits P1.4 and P1.5 before using
	while(1)
	{
            if(serialRead()=='A') //Press'A' to print P1.5 state
                if(P1OUT & BIT5) //If True, then On is printed
                serialPrint("ON");
            else
                serialPrint("OFF"); //If Flase, then Off is printed
        P1OUT ^= BIT5;       //Toggle P1.5 every .5 second
	    P1OUT ^= BIT4;      //Toggle P1.4 every .25 second(duty cycle=50%)
	    __delay_cycles(250000);
	    P1OUT ^= BIT4;
	    __delay_cycles(250000);
	}
	
	return 0;              //Unreachable state
}

