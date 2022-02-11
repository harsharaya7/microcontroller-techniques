#include <intrinsics.h>
#include <msp430g2553.h>
#include <templateEMP.h>

//Name: Rayaprolu Venkata Sai Harsha
//Matriculation Number: 4854097

int flag, pause_flag=0,playback_flag=0, led_state=0,ff_flag=0, rw_flag=0, k=0;  //ALL FLAGS USED FOR THE PROGRAM
void button_reset(void);
void button_check(void);
void sr_clock(void);
void playback(void);
void rewind(void);
void pause(void);
void fastforward(void);
void button_menu(void);
void bonus_rewind(void);

/**
 * button_check sees the state of the button everytime it is called.
 * flag=1 => PB3 (PLAYBACK)
 * flag=2 => PB2 (PAUSE)
 * flag=3 => PB1 (REWIND)
 * flag=4 => PB4 (FAST FORWARD)
 */


void button_check()   // STATE OF THE BUTTON
{
    button_reset();
    if(P2IN & BIT7)         //PB4 (Qd) -> flag=4
             flag=4;
         else
         {
             int i=0;
             P2OUT|= BIT2;
             P2OUT &=~BIT3;
             flag=-1;
             for(i=1;i<4;i++)
             {
                 sr_clock();
                 if(P2IN & BIT7)
                     flag=i;
                 P2OUT &=~BIT4;
             }
          }
    if(flag==4)
        fastforward();
    else if(flag==3)
        rewind();
  //  else
    //    playback();
}


void sr_clock() // Calls the clock of the Shift register
{
    P2OUT |=BIT4;
    P2OUT &=~BIT4;
}


/*
 * In playback(), I have used a flag(led_state) for the state of LED in order to make it work with
 * other states i.e. fast forward, pause and rewind
 * At every value of led_state, I have checked whether the value of the button has changed or not
 */


void playback()         // Playback
{
    button_check(); // To obtain the value of the button(flag)
    if(pause_flag==0 || playback_flag==1) // checks if the the Shift register is in pause state
    {
        //int i;
        P2OUT |=BIT0;
        P2OUT &= ~BIT1;
        P2OUT |=BIT6;
        if(flag==-1 && pause_flag==0)
            if(led_state==0)
            {
                P1OUT &= ~(BIT5 + BIT6);   //switch off red and blue LED (bonus)
                P1OUT |=BIT4;               //switch on green LED (bonus)

                P2OUT |=BIT0;
                P2OUT &=~BIT1;
                //P2OUT &=~BIT6;
                P2OUT |=BIT4;
                __delay_cycles(250000);   //delay of 250 milli seconds
                P2OUT &=~BIT4;
                //P2OUT &=~BIT6;
                led_state=1;
                button_check();             //obtains button(flag) value
                if(flag==2 || pause_flag==1)        //checks state of flag
                {
                     pause();
                     button_check();
                     return;
                }
                else if(flag==4)
                {
                    while(ff_flag==1)
                    fastforward();

                    return;
                }
                else if(flag==3)
                {
                    //while(rw_flag==1)
                    rewind();
                    return;
                }
            }
            if(led_state==1)
            {
                P1OUT &= ~(BIT4 + BIT6);
                P1OUT |=BIT5;

                P2OUT |=BIT0;
                P2OUT &=~BIT1;
                P2OUT &=~BIT6;
                P2OUT |=BIT4;
                __delay_cycles(250000);
                P2OUT &=~BIT4;
                led_state=2;
                button_check();
                if(flag==2 || pause_flag==1)
                {
                     pause();
                     button_check();
                     return;
                }
                else if(flag==4)
                {
                    while(ff_flag==1)
                    fastforward();

                    return;
                }
                else if(flag==3)
                {
                    //while(rw_flag==1)
                    rewind();
                    return;
                }
            }
            if(led_state==2)
            {
                P1OUT &= ~(BIT4 + BIT4);
                P1OUT |=BIT6;


                P2OUT |=BIT0;
                P2OUT &=~BIT1;
                P2OUT &=~BIT6;
                P2OUT |=BIT4;
                __delay_cycles(250000);
                P2OUT &=~BIT4;
                led_state=3;
                button_check();
                if(flag==2 || pause_flag==1)
                {
                     pause();
                     button_check();
                     return;
                }
                else if(flag==4)
                {
                    while(ff_flag==1)
                    fastforward();

                    return;
                }
                else if(flag==3)
                {
                    //while(rw_flag==1)
                    rewind();
                    return;
                }
            }
            if(led_state==3)
            {
                P1OUT &= ~(BIT5 + BIT6);
                P1OUT |=BIT4;

                P2OUT |=BIT0;
                P2OUT &=~BIT1;
                P2OUT &=~BIT6;
                P2OUT |=BIT4;
                __delay_cycles(250000);
                P2OUT &=~BIT4;
                led_state=0;
                button_check();
                if(flag==2 || pause_flag==1)
                {
                     pause();
                     button_check();
                     return;
                }
                else if(flag==4)
                {
                    while(ff_flag==1)
                    fastforward();

                    return;
                }
                else if(flag==3)
                {
                    //while(rw_flag==1)
                    rewind();
                    return;
                }
            }
        else
        button_check();

    }
}

void button_reset(void)   //parallel loading
{
    P2OUT |=BIT2+BIT3;
    P2OUT &= ~BIT0;
    P2OUT &= ~BIT1;
    sr_clock();
}
/**
 * main.c
 */
int main(void)
{
    initMSP();              //initialise microcontroller
    flag=-1;                //initialise flag=-1 (button=-1)
    WDTCTL = WDTPW + WDTHOLD;       //Watchdog in hold
    P2DIR |= 0xFF;          //All the pins of shift register are made as output
    P1SEL= 0x00;            //All pins of Port1
    P1SEL2=0X00;            //..in I/O configuration

    P1DIR |= (BIT4 + BIT5 + BIT6);   // Pins P1.4, P1.5, P1.6 used for bonus directed as output

    P1OUT &=~(BIT4 + BIT5 +BIT6);   //Clearning pins P1.4, P1.5, P1.6 before using

    P2DIR &= ~BIT7;                 //Making P2.7 as Input(value of button)
    P2SEL = 0x00;                   // All pins of Port2..
    P2SEL2 =0X00;                   //..configured as I/O
    P2OUT &= ~ BIT5 ;               // CLR
    P2OUT |= BIT5 ;                 //CLR of shift register set to 1 before program
    P2OUT |= BIT0 ;                 // Shift register set to shift right (S0 = 1 , S1=0 (Shift register 1))
    P2OUT &= ~ BIT1 ;

    P2OUT &= ~ BIT5 ; //added
    P2OUT |= BIT5 ;     //added

    // apply the data to be shifted at SR (SR = 1 to insert a 1).

    sr_clock();

    P2OUT &= ~BIT5;
    P2OUT |= BIT5;
    while(1)
    {
        button_check();
        if(flag==1)
            while(flag==1 || playback_flag==1)
            playback_continue();
        else if(flag==3)
            rewind();
        else if(flag==4)
            fastforward();
        button_reset();

    }
	return 0;
}




void playback_continue()            //Used to call playback(Called it for convenience in main function))
{
    playback_flag=1;
    pause_flag=0;
    playback();
}

void pause()
{
    pause_flag=1;
    playback_flag=0;
    P2OUT &=~BIT0;              // Shift Register2 S0=0 and S1=0 (Hold state)
    P2OUT &=~BIT1;
    sr_clock();
    flag=0;
}

/*
 * fastforward(), is similar to playback(), with more flags to be checked and less delay
 */

void fastforward()
{
        P2OUT |=BIT0;
        P2OUT &= ~BIT1;
        P2OUT |=BIT6;
        if(flag==4) //&& pause_flag==0)
        {
            if(led_state==0 && flag==4)
            {
                P1OUT &= ~(BIT5 + BIT6);
                P1OUT |=BIT4;

                P2OUT |=BIT0;
                P2OUT &=~BIT1;
                //P2OUT &=~BIT6;
                P2OUT |=BIT4;
                __delay_cycles(125000);         // delay of 125 milli seconds
                P2OUT &=~BIT4;
                //P2OUT &=~BIT6;
                led_state=1;
                button_check();
                if(flag!=4 && playback_flag==1)
                    playback();
                else if(flag!=4 && pause_flag==1)
                    pause();
            }
            if(led_state==1 && flag==4)
            {
                P1OUT &= ~(BIT4 + BIT6);
                P1OUT |=BIT5;

                P2OUT |=BIT0;
                P2OUT &=~BIT1;
                P2OUT &=~BIT6;
                P2OUT |=BIT4;
                __delay_cycles(125000);
                P2OUT &=~BIT4;
                led_state=2;
                button_check();
                if(flag!=4 && playback_flag==1)
                     playback();
                 else if(flag!=4 && pause_flag==1)
                     pause();

            }
            if(led_state==2 && flag==4)
            {
                P1OUT &= ~(BIT4 + BIT5);
                P1OUT |=BIT6;

                P2OUT |=BIT0;
                P2OUT &=~BIT1;
                P2OUT &=~BIT6;
                P2OUT |=BIT4;
                __delay_cycles(125000);
                P2OUT &=~BIT4;
                led_state=3;
                button_check();
                if(flag!=4 && playback_flag==1)
                       playback();
                else if(flag!=4 && pause_flag==1)
                       pause();

            }
            if(led_state==3&& flag==4)
            {
                P1OUT &= ~(BIT5 + BIT6);
                P1OUT |=BIT4;

                P2OUT |=BIT0;
                P2OUT &=~BIT1;
                P2OUT &=~BIT6;
                P2OUT |=BIT4;
                __delay_cycles(125000);
                P2OUT &=~BIT4;
                led_state=0;
                button_check();
                if(flag!=4 && playback_flag==1)
                       playback();
                else if(flag!=4 && pause_flag==1)
                       pause();

            }
        }
        button_check();
        if(flag==4)
            ff_flag=1;
        else
            ff_flag=0;
}

/*
 * rewind(): checks the led_state and left shifts depending on the state of led_state
 */

void rewind()
{
    int i=0;
    if(flag==3) //&& pause_flag==0)
    {
        if(led_state == 0)
        {

                P2OUT |= BIT0;
                P2OUT &= ~BIT1;
                P2OUT |= BIT6;
                sr_clock();
                P2OUT &= ~BIT6;
                while(i<2)
                 {
                    sr_clock();
                    i++;
                 }

                led_state = 4;
        }
        else if(led_state==1)
        {

            P2OUT |= BIT0;
            P2OUT &= ~BIT1;
            P2OUT |= BIT6;
            sr_clock();
            P2OUT &= ~BIT6;
            while(i<2)
            {
                 sr_clock();
                 i++;
            }

             led_state = 4;

        }
        else
        {
            P2OUT &= ~BIT0;             // Left shift operation (S0=0 S1=1)
            P2OUT |= BIT1;
            P2OUT &= ~BIT6;
            led_state--;
         }

        sr_clock();
        k++;
        bonus_rewind();
        __delay_cycles(125000);
        button_check();
    }
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;

}

/*
 * bonus_rewind() is for the rewind functioning of D5, D6, D7
 */


void bonus_rewind()
{
    if(k==1)
    {
        P1OUT &= ~(BIT5 + BIT6);
        P1OUT |=BIT4;
    }
    else if(k==2)
    {
        P1OUT &= ~(BIT4 + BIT5);
        P1OUT |=BIT6;

    }
    else if(k==3)
    {
        P1OUT &= ~(BIT4 + BIT6);
        P1OUT |=BIT5;

    }
    else if(k==4)
    {
        P1OUT &= ~(BIT5 + BIT6);
        P1OUT |=BIT4;
        k=0;
    }
}










