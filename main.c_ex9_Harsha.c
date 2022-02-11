#include <intrinsics.h>
#include <msp430g2553.h>
#include <templateEMP.h>
#include <stdlib.h>
//Name: Rayaprolu Venkata Sai Harsha
//Matriculation Number: 4854097
//Name: Tiruvaipati Sai Sourabh
//Matriculation Number: 4651358

/*   GPIO COMMUNICATION ---> P1.3 - P1.7      */
/*   GREEN LED ----> P3.0                     */
/*   SEND AND RECEIVE BIT ----> P3.1          */
/*   RED LED -----> P3.2                      */
/*   BUZZER -------> P3.6                     */
/*   GND -------> GND                         */


void button_reset(void);
void button_check(void);
void sr_clock(void);
void sr_clear(void);
int number(void);
void check_array(void);
void encode(void);
void led_on(void);
void led_clear(void);
void gpio_in(void);
void watchdog_init(void);
void buzzer(void);
volatile int numbers;
volatile int correct_flag;
int guess_array[4], led_array[4];
int button_press1 = 0, button_press2 = 0, button_press3 = 0, button_press4 = 0;
short send_hex, send_rem;
int flag = -1, k = 0, RNG_flag = -1, turns = 0, i = 0, led_state = 0, break_flag; //ALL FLAGS USED FOR THE PROGRAM

const unsigned int Mastermind_values[24][4] = {{1, 2, 3, 4}, {1, 2, 4, 3}, {1, 3, 4, 2}, {1, 3, 2, 4}, {1, 4, 2, 3}, {1, 4, 3, 2}, {2, 1, 3, 4}, {2, 1, 4, 3}, {2, 3, 4, 1}, {2, 3, 1, 4}, {2, 4, 1, 3}, {2, 4, 3, 1}, {3, 1, 2, 4}, {3, 1, 4, 2}, {3, 2, 4, 1}, {3, 2, 1, 4}, {3, 4, 1, 2}, {3, 4, 2, 1}, {4, 2, 3, 1}, {4, 2, 1, 3}, {4, 3, 1, 2}, {4, 3, 2, 1}, {4, 1, 2, 3}, {4, 1, 3, 2}}; //All the 24 possibilities

const unsigned int note1[] = {587, 523, 932, 523, 587, 0}; //Winner notes

void watchdog_init(void) //to restart the the game
{
    BCSCTL3 |= LFXT1S1; //AUXILLARY CLOCK
    BCSCTL1 |= DIVA0;   // DIVIDED ACLOCK
    WDTCTL = WDTPW + WDTCNTCL + WDTSSEL;
}

void buzzer(void) //Buzzer that indicates win scenario
{
    int pwm = 0;
    P3DIR |= BIT6;
    P3SEL |= BIT6;
    P3REN &= ~BIT6;
    for (pwm = 0; pwm <= 5; pwm++)
    {
        TA0CCTL2 = OUTMOD_3;
        TA0CCR0 = note1[pwm];
        TA0CCR2 = note1[pwm] / 2; // CCR2 PWM duty cycle (50 %)
        TA0CTL = TASSEL_2 + MC_1; // SMCLK ; MC_1 -> up mode ;
        __delay_cycles(250000);
    }
}

void gpio_in(void) //Makes communication bit as input
{
    P3DIR &= ~BIT1;
    P3REN |= BIT1;
    P3OUT &= ~BIT1;
}

void sr_init(void) //shift register initialisation
{
    P2DIR |= 0xFF;
    P2DIR &= ~BIT7;
    P2SEL = 0x00;
    P2SEL2 = 0X00;
    P2OUT &= ~BIT5;
    P2OUT |= BIT5;
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;
    P2OUT &= ~BIT5;
    P2OUT |= BIT5;
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;
    P2OUT |= BIT6;
}

void button_check() //buttons of shift register
{
    button_reset();
    if (P2IN & BIT7)
    {
        flag = 4;
        P3OUT &= ~BIT0;
        __delay_cycles(100000);
    }
    else
    {

        P2OUT |= BIT2;
        P2OUT &= ~BIT3;
        flag = -1;
        for (i = 1; i < 4; i++)
        {
            sr_clock();
            if (P2IN & BIT7)
            {
                flag = i;
                P3OUT &= ~BIT0;
                __delay_cycles(100000);
            }
            P2OUT &= ~BIT4;
        }
    }
}

void sr_clock() // Calls the clock of the Shift register
{
    P2OUT |= BIT4;
    P2OUT &= ~BIT4;
}

void sr_clear(void) //clears the shift register LEDs
{
    P2OUT &= ~BIT5;
    P2OUT |= BIT5;
}

void button_reset(void) //parallel loading
{
    P2OUT |= BIT2 + BIT3;
    P2OUT &= ~BIT0;
    P2OUT &= ~BIT1;
    sr_clock();
}
/**
 * main.c
 */
int main(void)
{
    initMSP();                //initialise microcontroller
    WDTCTL = WDTPW + WDTHOLD; //Watchdog in hold
    P1DIR |= BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
    P3DIR |= BIT0 + BIT2;
    P3OUT &= ~(BIT0 + BIT2);
    P1OUT &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    sr_init();
    RNG_flag = number(); // Generates random number
    while (1)
    {
        P3OUT &= ~BIT0;
        while (turns < 3) //number of turns per player
        {
            led_clear();
            P3OUT |= BIT0;
            button_check();
            if (flag == 1 && button_press1 == 0)
            {
                guess_array[k] = 3;
                k++;
                button_press1 = 1;
                flag = 0;
            }
            else if (flag == 3 && button_press2 == 0)
            {
                guess_array[k] = 1;
                k++;
                button_press2 = 1;
                flag = 0;
            }
            else if (flag == 4 && button_press3 == 0)
            {
                guess_array[k] = 4;
                k++;
                button_press3 = 1;
                flag = 0;
            }
            else if (flag == 2 && button_press4 == 0)
            {
                guess_array[k] = 2;
                k++;
                button_press4 = 1;
                flag = 0;
            }
            button_reset();
            if (k == 4)
            {
                check_array();
                led_on();
                k = 0;
                button_press1 = 0, button_press2 = 0, button_press3 = 0, button_press4 = 0;
                P3OUT &= ~BIT0;
                turns++;
                __delay_cycles(3000000);
            }
        }
        if (turns == 3)
        {
            encode();
            while (break_flag == 0) //loop till an input from board 2
            {
                gpio_in();
                if (P3IN & BIT1)
                {
                    break_flag = 1;
                }
                else
                    break_flag = 0;
            }
            if (correct_flag >= 4) //if player wins, buzzer rings
            {
                buzzer();
            }
            else //if player loses, RED LED is switched on
            {
                P3OUT |= BIT2;
            }
            watchdog_init(); //watchdog initialisation
            while (1)
            {
                //infinite loop to trigger watchdog
            }
        }
    }
    return 0;
}

void check_array(void)
{
    correct_flag = 0;
    int j;
    for (j = 0; j < 4; j++)
    {
        if (guess_array[j] == Mastermind_values[RNG_flag][j])
        {
            led_array[j] = 1; //Debugging array
            correct_flag++;   //correct number of states
        }
        else
            led_array[j] = 0;
    }
    if (correct_flag == 4)
        turns = 2;
}

void led_on(void) //switches on number of matched LEDs
{

    sr_init();
    P2OUT |= BIT6;
    sr_clear();
    for (led_state = 0; led_state < correct_flag; led_state++)
    {
        sr_clock();
    }
}

void led_clear() //clears the LEDs of shift register
{
    int x = 0;
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;
    P2OUT &= ~BIT6;
    while (x < 5)
    {
        sr_clock();
        x++;
    }
}

void encode(void) //encoding the value of RNG_flag to send board 2
{
    send_hex = RNG_flag / 2;
    send_hex <<= 4;
    send_rem = RNG_flag % 2;
    P1OUT |= send_hex;

    if (send_rem == 1)
    {
        P1OUT |= BIT3;
    }
    else
    {
        P1OUT &= ~BIT3;
    }
}

int number() //Function to generate random numbers
{
    srand(time(NULL));           //initialize the random see
    int RandIndex = rand() % 24; //generates a random number between 0 and 23
    numbers = RandIndex;
    return numbers;
}

