#include <msp430.h>
#include <templateEMP.h>


/**
 * Name: Rayaprolu Venkata Sai Harsha
 * Matriculation number: 4854097
 */


int note1[]= {587, 523,932,523,587,567, 600,669,721,779,0};
int note2[]={660,660,660,510,660,770,380,771,660,881,820,921,965,123,0};
unsigned int i;
volatile int j=0;

//#define interrupt               //enable this for interrupt part


void playnote_1(void);
void playnote_2(void);
void playnote_0(void);
void init_PB5(void);
void playnote(void);
void pause(void);

volatile int pause_flag=0;      //flags used
volatile int return_flag=0;

void buzzer_init(void);

void buzzer_init(void)
{
    P3SEL &= ~BIT6;
    P3SEL2 &= ~BIT6;
    P3DIR &= ~BIT6;
    P3REN |= BIT6;
    P3OUT |=BIT6;
}


/**
 * main.c
 */
void main(void)
{
    initMSP();
    P1SEL= 0x00;        //initiate I/O configuration
    P1SEL2= 0x00;

    TA0CTL = TASSEL_2 + MC_1 ;
#ifdef interrupt
    init_PB5();
#endif

    TA0CCTL2 = OUTMOD_3 ; // CCR2 set / reset
    while(1)
    {
#ifndef interrupt
        P3DIR |= BIT6 ;
        P3SEL |= BIT6 ;
        P3REN &=~BIT6;
        P1DIR &=~BIT4;
        P1REN |=BIT4;
        P1OUT |=BIT4;
        playnote_0();
#endif
        P1DIR &=~BIT4;      //P1.4 (PB6) as pause
        P1REN |=BIT4;
        P1OUT |=BIT4;
        buzzer_init();      //Make P3.6 as input
//        __delay_cycles(50000);

     if((!(P3IN & BIT6)))
     {
         __delay_cycles(150000);    //debounch for 150ms
         //P1OUT |=BIT7;
         int piezo_flag=1 ;         //flag to check number of vibrations
         long int count=30000;
         while(count > 0)
         {
             count--;
             if(!(P3IN & BIT6))
             {
                 piezo_flag=2;

                 __delay_cycles(100000);
             }
         }
         P3DIR |= BIT6 ; //Make buzzer output
         P3SEL |= BIT6 ;
         P3REN &=~BIT6;
         if(piezo_flag==1)
         {
             playnote_1();
         }
         else if(piezo_flag==2)
         {
             playnote_2();
         }
         piezo_flag=0;


        // __delay_cycles(500000);

     }
    }


}

void playnote_0(void)           //Both tunes without interrupt
{
    for(i=0;i<=10;i++)
    {
        pause_flag=0;
        TA0CCR0 = note1[i]; // PWM Period : 1000 us
        TA0CCR2 = note1[i]/2; // CCR2 PWM duty cycle (50 %)
        TA0CTL = TASSEL_2 + MC_1 ; // SMCLK ; MC_1 -> up mode ;
        __delay_cycles(250000);
       // TA0CCTL2=OUTMOD_5;
        pause();


    }
    TA0CCR0 = 0;
    __delay_cycles(1000000);
    for(i=0;i<=14;i++)
    {
        pause_flag=0;
        TA0CCR0 = note2[i]; // PWM Period : 1000 us
        TA0CCR2 = note2[i]/2; // CCR2 PWM duty cycle (50 %)
        TA0CTL = TASSEL_2 + MC_1 ; // SMCLK ; MC_1 -> up mode ;
        __delay_cycles(250000);
        pause();
    }
    TA0CCR0 = 0;
    __delay_cycles(1000000);
}


void init_PB5(void)     //Initialise interrupt
{
    P1DIR &= ~ BIT3 ; // Set as input
    P1REN |= BIT3 ; // Enable pull - resistors
    P1OUT |= BIT3 ; // Set to pull -up
    P1IE |= BIT3 ; // Enable interrupt
    P1IES |= BIT3 ; // High /Low - Edge
    P1IFG &= ~ BIT3 ; // Clear interrupt flag

 /*
  * DId not use P1.4 as interrupt
  */


//    P1DIR &=~BIT4;
//    P1REN |= BIT4 ; // Enable pull - resistors
//    P1OUT |= BIT4 ; // Set to pull -up
//    P1IE |= BIT4 ; // Enable interrupt
//    P1IES |= BIT4 ; // High /Low - Edge
//    P1IFG &= ~ BIT4 ; // Clear interrupt flag

}

/*
 *  I used polling to read P1.4 (PB6), as interrupt for P1.4 was not working for my code
 */

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

    P1IE &=~BIT3;
    P3DIR |= BIT6 ; //Make buzzer output (P3.6)
    P3SEL |= BIT6 ;
    P3REN &=~BIT6;
    P1DIR &=~BIT4;  //Make  P1.4 input
    P1REN |=BIT4;
    P1OUT |=BIT4;
    volatile int flag=-1;
    if(j==0)        //initially the program falls into interrupt
    {               //in order to remove that effect, I have done this routine
        flag=0;
        j++;
    }
    else
    {
        flag=-1;
    }

    if(!(P1IN & BIT3))
    {
        __delay_cycles(300000);     // check for other input
        if(!(P1IN & BIT3))
            flag=1;
    }
   __delay_cycles(1000000);          //wait for 1sec

    if(flag==1)
    {
        for(i=0;i<=14;i++)
        {
            pause_flag=0;
            TA0CCR0 = note2[i]; // PWM Period : 1000 us
            TA0CCR2 = note2[i]/2; // CCR2 PWM duty cycle (50 %)
            TA0CTL = TASSEL_2 + MC_1 ; // SMCLK ; MC_1 -> up mode ;

            __delay_cycles(250000);
            pause();
        }

        flag=0;
    }
    else if(flag==-1)
    {
        for(i=0;i<=10;i++)
        {
           pause_flag=0;
           TA0CCTL2= OUTMOD_3;
           TA0CCR0 = note1[i]; // PWM Period : 1000 us
           TA0CCR2 = note1[i]/2; // CCR2 PWM duty cycle (50 %)
           TA0CTL = TASSEL_2 + MC_1; //SMCLK ; MC_1 -> up mode ;

           __delay_cycles(250000);
           pause();

        }
        flag=0;
    }
    P1IE |=BIT3;
    P1IFG &= ~ BIT3 ;


}

void pause(void)
{


    while((!(P1IN & BIT4)) || pause_flag==1)
    {

        pause_flag=1;
        __delay_cycles(250000); // increase this later (250ms)
       // TA0CCTL2= OUTMOD_5;
          TA0CCR0 = 0;
        //while(!(P1IN & BIT4))
        if(!(P1IN & BIT4))
        {
            break;
        }
#ifdef interrupt
        else if(!(P1IN & BIT3))
        {
            return_flag=-1;
            __delay_cycles(300000);
            if(!(P1IN & BIT3))
                return_flag=1;
            if(return_flag==-1)
            {
                playnote_1();
                break;
            }
            else if(return_flag==1)
            {
                playnote_2();
                break;
            }

        }
#endif

    }
}



void playnote_2(void)
{
    for(i=0;i<=14;i++)
            {
                pause_flag=0;
                TA0CCR0 = note2[i]; // PWM Period : 1000 us
                TA0CCR2 = note2[i]/2; // CCR2 PWM duty cycle (50 %)
                TA0CTL = TASSEL_2 + MC_1 ; // SMCLK ; MC_1 -> up mode ;

                __delay_cycles(250000);
                pause();
            }
}
void playnote_1(void)
{
    for(i=0;i<=10;i++)
    {
       pause_flag=0;
       TA0CCTL2= OUTMOD_3;
       TA0CCR0 = note1[i]; // PWM Period : 1000 us
       TA0CCR2 = note1[i]/2; // CCR2 PWM duty cycle (50 %)
       TA0CTL = TASSEL_2 + MC_1; //SMCLK ; MC_1 -> up mode ;

       __delay_cycles(250000);
       pause();

    }
}



