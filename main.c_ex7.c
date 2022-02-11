/*
 * Name: Rayaprolu Venkata Sai Harsha
 * Matriculation number: 4854097
 */


//#include <msp430.h>
#include<templateEMP.h>
#define task1               //TASK 1
//#define task2             //Uncomment for all of task2
//#define task2e            //Uncomment for task2e
/**
 * main.c
 */
volatile int button_flag=0, timer_flag=0, m1=0,i=0 , check=0;;


void button_init(void);
void heater_init(void);
void sr_init(void);
void sr_clear(void);
void sr_clock(void);
void heater(void);
void heater_check(int);
void timer_init(void);
void blink();


void blink(void)
{
    P1OUT |=BIT0;
    __delay_cycles(125000);         /// Not 4HZ
    P1OUT &=~BIT0;
    __delay_cycles(125000);         /// Not 4HZ
}


void timer_init(void)           //timer initialisation
{
    TA0CCR0 |= 1000;
    TA0CTL |= TASSEL_2 + MC_1;
    TACCTL0 |= CCIE;
}
void sr_clear(void)            //shift register initialisation
{
    P2OUT &= ~ BIT5 ;
    P2OUT |= BIT5 ;

}

void sr_clock(void)
{
    P2OUT |= BIT4;
    P2OUT &=~BIT4;
}

void sr_init(void)           //Shift register initialisation similar to the previous exercise
{                           //.. (similar one from Exercise_3_Rayaprolu)
    P2DIR |= 0xFF;
    P2DIR &= ~BIT7;
    P2SEL = 0x00;
    P2SEL2 =0X00;
    P2OUT &= ~ BIT5 ;
    P2OUT |= BIT5 ;
    P2OUT |= BIT0 ;
    P2OUT &= ~ BIT1 ;

    P2OUT &= ~ BIT5 ;
    P2OUT |= BIT5 ;

    P2OUT |=BIT0;
    P2OUT &= ~BIT1;
    P2OUT |=BIT6;

}

void heater_init(void)          //heater initialisation
{
#ifndef task2e
    P3DIR |= BIT4;
    P3OUT &=~BIT4;
#endif
    ADC10CTL0 = ADC10ON + ADC10SHT_2;
    ADC10AE0 |= BIT5;
    ADC10CTL1 = INCH_5 ;             // Select channel 5 as input for the following conversion (s)
}
void button_init(void)          //PB5 interrupt initialisation
{
    P1DIR &= ~ BIT3 ; // Set as input
    P1REN |= BIT3 ; // Enable pull - resistors
    P1OUT |= BIT3 ; // Set to pull -up
    P1IE |= BIT3 ; // Enable interrupt
    P1IES |= BIT3 ; // High /Low - Edge
    P1IFG &= ~ BIT3 ; // Clear interrupt flag
}
int main(void)
{
	initMSP();
#ifdef task2
    heater_init();
    sr_init();
    sr_clear();
    timer_init();
#endif
	BCSCTL3 |= LFXT1S1; //AUXILLARY CLOCK
#ifdef task1
	BCSCTL1 |= DIVA0; // DIVIDED ACLOCK
#endif
#ifdef task2
	BCSCTL1 |= DIVA_1 | DIVA_2;     //       +DIVA1;
#endif

#ifdef task2e
    P3DIR |= BIT5;
    P3SEL |= BIT5;
    TA0CCTL1 = OUTMOD_3;
    TA0CCR0 = 1000;
#endif


	WDTCTL = WDTPW + WDTCNTCL + WDTSSEL;   //WRITE WATCHDOG TIMER THINGY

	button_init();
	P1DIR |= BIT0;
	P1OUT |=BIT0;
	P3DIR |= BIT2;
	P3OUT &=~BIT2;
	//P3OUT &= ~BIT2;
	//P1OUT &=~BIT0;
	while(1)
	{

	    WDTCTL = WDTPW + WDTCNTCL + WDTSSEL;
//#ifndef task2e
//    if(m1 < 450)
//        TA0CCR1 = 500;
//    else if(m1 > 530)
//        TA0CCR1 = 0;
//#endif
        if(button_flag==1)
        {
            while(1)
            {

            }

        }
        blink();
#ifdef task2
        heater_check(m1);
#endif
	}

	return 0;
}


void heater(void)
{
    int j=0;
    //Start conversion
    ADC10CTL0 |= ENC + ADC10SC ;
    //Wait until result is ready
    while ( ADC10CTL1 & ADC10BUSY );
    m1 = ADC10MEM;
    if(m1 >= 340 && m1<390 )
    {

        if(i==0)
        {
            P3OUT &=~BIT2;
            sr_clear();
            sr_clock();
            i=1;
        }
        //heater_check(m1);
    }
    else if(m1>=390 && m1<440)
    {

        if(i==1 || i==0)
        {
            P3OUT &=~BIT2;
            sr_clear();
            for(j=0;j<=1;j++)
            sr_clock();
            if(i!=0)
            i=2;
        }

    }
    else if(m1>=440 && m1<490)
    {

        if(i==2 || i==0)
        {
            P3OUT &=~BIT2;
            sr_clear();
            for(j=0;j<=2;j++)
            sr_clock();
            if(i!=0)
            i=3;
        }

    }
    else if(m1>=490 && m1<540 )
    {

        if(i==3|| i==0)
        {
            P3OUT &=~BIT2;
            sr_clear();
            for(j=0;j<=3;j++)
            sr_clock();
            if(i!=0)
                i=4;
        }
    }
    else if(m1>=550)
    {
        if(i==4)
        {
            P3OUT |=BIT2;
            i=0;
        }
        else if(i==0)
        {
            P3OUT |=BIT2;
            sr_clear();
            for(j=0;j<=3;j++)
            sr_clock();
            i=0;
        }
    }
    serialPrintInt(m1);

}
void heater_check(int check)
{
//    serialPrintln("Here");
//    serialPrintInt(m1);
//    serialPrintln("");
#ifndef task2e
    P3OUT |= BIT4;
#endif
    if(check<450)
    {
#ifndef task2e
        P3OUT |=BIT4;
#endif
        i=0;
#ifdef task2e
        TA0CCR1 = 500;
#endif
    }
    else if(check>530)
    {
#ifndef task2e
        P3OUT &=~BIT4;
#endif
        i=0;
#ifdef task2e
        TA0CCR1 = 0;
#endif
    }
}


# pragma vector = PORT1_VECTOR
__interrupt void Port_1 ( void )
{
    P1IE &=~BIT3;
    __delay_cycles(25000);
    P1IE |=BIT3;
    button_flag=1;
    i=0;
    P1IFG &=~BIT3;
}


# pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A ( void )
{
    if(timer_flag>2000)
    {
        heater();
        timer_flag=0;
    }
    else
        timer_flag++;
#ifndef task2e
    if(m1 < 450)
        TA0CCR1 = 500;
    else if(m1 > 530)
        TA0CCR1 = 0;
#endif
}



