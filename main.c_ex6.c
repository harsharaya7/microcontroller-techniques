/**
 * Name: Rayaprolu Venkata Sai Harsha
 * Matriculation number: 4854097
 *
 *
 * ****** PINS *******
 *
 * BUZZER ---> RELAY MIDDLE
 * DAC_IN ---> RELAY RIGHT
 * P3.6 (PWM_PIN) ---> RELAY LEFT
 * P1.3(INTERRUPT__PIN) ---> COMP_OUT
 * P1.4 ---> REL_STAT
 *
 * *******************
 */

#include <msp430.h>
#include <templateEMP.h>

int note1[]= {587, 523,932,523,587,567, 600,669,721,779,0};
int note2[]={660,660,660,510,660,770,380,771,660,881,820,921,965,123,0};
unsigned int i;

volatile int knock_flag=0;

void playnote_1(void);
void playnote_2(void);
void init_knock(void);


void buzzer_init(void);

void buzzer_init(void)
{
    P3DIR |= BIT6;
    P3SEL |=BIT6;
    P3REN &=BIT6;
}


/**
 * main.c
 */
void main(void)
{
    initMSP();
    buzzer_init();
    init_knock();
    P1DIR |= BIT4;
    P1OUT |=~BIT4;
    P1OUT &=~BIT4;
    P3OUT |=BIT6;

    while(1)
    {
        if(knock_flag==1)
        {
            P1OUT |=BIT4;
            playnote_1();
            P1OUT &=~BIT4;
            P1IE |=BIT3;
            knock_flag=0;
        }
        else if(knock_flag==2)
        {
            P1OUT |=BIT4;
            playnote_2();
            P1OUT &=~BIT4;
            P1IE |=BIT3;
            knock_flag=0;
        }
    }
}


void playnote_2(void)
{
    P1IE &= ~BIT3;
    for(i=0;i<=14;i++)
    {
        TA0CCR0 = note2[i]; // PWM Period : 1000 us
        TA0CCR2 = note2[i]/2; // CCR2 PWM duty cycle (50 %)
        TA0CTL = TASSEL_2 + MC_1 ; // SMCLK ; MC_1 -> up mode ;
        __delay_cycles(250000);
    }
}
void playnote_1(void)
{
    P1IE &= ~BIT3;
    for(i=0;i<=10;i++)
    {
       TA0CCTL2= OUTMOD_3;
       TA0CCR0 = note1[i]; // PWM Period : 1000 us
       TA0CCR2 = note1[i]/2; // CCR2 PWM duty cycle (50 %)
       TA0CTL = TASSEL_2 + MC_1; //SMCLK ; MC_1 -> up mode ;

       __delay_cycles(250000);
    }
}

void init_knock(void)     //Initialise interrupt
{
    P1DIR &= ~ BIT3 ; // Set as input
    P1REN |= BIT3 ; // Enable pull - resistors
    P1OUT |= BIT3 ; // Set to pull -up
    P1IE |= BIT3 ; // Enable interrupt
    P1IES |= BIT3 ; // High /Low - Edge
    P1IFG &= ~ BIT3 ; // Clear interrupt flag
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1IE &=~BIT3;
    __delay_cycles(250000);
    P1IE |=BIT3;
    if(knock_flag==1)
    {
        knock_flag=2;
        P1IFG &=~BIT3;
        __delay_cycles(200000);
    }
    else
    {
        knock_flag=1;
        P1IFG &=~BIT3;
        __delay_cycles(200000);
    }


}
