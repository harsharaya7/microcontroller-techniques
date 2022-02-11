/* Name: Rayaprolu Venkata Sai Harsha
 * Matriculation number: 4854097
 */


#include <intrinsics.h>
#include <msp430g2553.h>
#include <templateEMP.h>
//#define polling
#define B1 BIT3  //define PB5 as B1
#define B2 BIT4  //define PB6 as B2
#define D5 BIT6  //define GREEN LED as D5
#define D6 BIT5  //define RED LED as D6
#define D7 BIT0  //define BLUE LED as D7
#define D9 BIT7  //define YELLOW as D9

void initInterrupt(); //declare interrupt initialization
void initInterrupt()  //define interrupt  initialization
{
P1DIR &=~B1;      //declare B1 as input
P1REN |=B1;       //resistor enabled
P1OUT |= B1;      //declare pullup resistor
P1IES |=B1;       //High/Low-edge
P1IE |= B1;       //Enable Interrupt
P1IFG &=~B1;      //Clear interrupt flag
/* Declared as given in the exercise2 sheet*/
}

void state_D6_D9(); //declare separate function for Red and Yellow LED

void state_D6_D9()  //define separate function for Red and Yellow LED
{
    if(!(P1IN & B1))  //state PB5=1 and PB6=0
     {
         P1OUT &=~D9;
         P1OUT |=D6;
         __delay_cycles(250000);
         P1OUT &=~D6;
         P1OUT |=D9;
         __delay_cycles(250000);

     }
     else if(!(P1IN & B2))  //state PB5=0 and PB6=1
     {
         P1OUT |= D9;
         P1OUT &=~D6;
     }
     else if(!(P1IN&B1))        //state PB5=1 and PB6=0 (CHANGES TO PB5=1 AND PB6=1 AND CHANGES BACK TO PB5=1 AND PB6=1)
     {
          while(!(P1IN & B2))   //checks for PB6=1
          {
                P1OUT |=D9;
                P1OUT &=~D6;
          }
     }
     else   //state PB5=0 and PB6=0
     {
          P1OUT |=D9;
          P1OUT &=~(D6);
      }
}



/**
 * main.c
 */


int main(void)
{
    initMSP();          //initiate micro-controller
    #ifndef polling
    initInterrupt();    //initiate interrupt when polling is disabled
    #endif
    P1SEL= 0x00;        //initiate I/O configuration
    P1SEL2= 0x00;
    P1DIR |= D5;        //configure D5, D6, D7, D9 as output
    P1DIR |= D6;
    P1DIR |= D7;
    P1DIR |= D9;
    P1DIR &= ~B1;       //configure B1 and B2 as inputs
    P1DIR &= ~B2;
    P1REN |= B1;        //pullup resistor enabled for B1 and B2
    P1REN |= B2;
    P1OUT |= B1;        //set pullup resistor for B1 and B2
    P1OUT |= B2;
    P1OUT &= ~(D5 +D6+ D7+ D9);     //clear all LEDs before using them
    while(1)
    {

        if(!(P1IN & B1))        //B1 is at level high
        {
            __delay_cycles(250000);  //debounce 250millisecond to check for B2

            while(!(P1IN & B2))         //checks for B2
            {
                   P1OUT |=(D7+D9);
                   P1OUT &=~(D5+D6+D7+D9);
                   //state here B1=1 and B2=1 and jumps back to  B1=1 and B2=0 when button is released
            }
            P1OUT &=~(D5+D6+D7+D9);  // clears previous states
            P1OUT &=~(D5+D7);        //clears state of B1=1 and B2=1
            state_D6_D9();          //call explicit function for red and yellow LED
        }
        else if(!(P1IN & B2))       //B2 at level high
        {
            __delay_cycles(250000);  //debounce 250millisecond

            while(!(P1IN & B1))         //checks for B1=1
            {
                   P1OUT |=(D7+D9);
                   P1OUT &=~(D5+D6+D7+D9);
                   //state here B1=1 and B2=1 and jumps back to  B1=1 and B2=0 when button is released
            }
            P1OUT &=~(D5+D6+D7+D9);    //clear previous outputs
            P1OUT |= D5;               //set D5 as 1
            P1OUT &=~(D7);              //clear D7
            state_D6_D9();             //call explicit function for red and yellow LED
        }

        else
        {
            P1OUT &=~(D5+D7);         //clear D5 and D7
            state_D6_D9();            //call explicit function for red and yellow LED
                //state here B1=0 and B2=0
        }


    }
   	return 0;
}


#pragma vector=PORT1_VECTOR   //port1 interrupt vector
__interrupt void Port_1(void)
{
if(!(P1IN & B1))            //B1 in high level
{
    if(!(P1IN & B2))        //checks whether B2 is high
    {
        P1OUT |= D7;        //set D7
       //state here B1=1 and B2=1
    }
    else if(P1IN & B2)      //checks whether B2 is low
    {
        state_D6_D9();     //calls explicit function for Red and Yellow LED
    }
}
P1IFG &=~B1;            //clear interrupt flag
}


