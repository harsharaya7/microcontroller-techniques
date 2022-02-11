#include <msp430g2553.h>
#include <templateEMP.h>
#include <stdlib.h>

//Tiruvaipati : 4651358
//Rayaprolu: 4854097

/*   GPIO COMMUNICATION ---> P1.3 - P1.7      */
/*   GREEN LED ----> P3.0                     */
/*   SEND AND RECEIVE BIT ----> P3.1          */
/*   RED LED -----> P3.2                      */
/*   BUZZER -------> P3.6                     */
// Connect GND on both boards

int flag, k = 0, break_flag, turns, led_state = 0; //ALL FLAGS USED FOR THE PROGRAM
void loser(void);
void buzzer(void);
void button_reset(void);
void button_check(void);
void sr_clock(void);
void sr_clear(void);
int number(void);
void check_array(void);
void decode(void);
void led_on(void);
void led_clear(void);
void watchdog_init(void);
void gpio_out(void);
volatile int numbers;
volatile int correct_flag;

int guess_array[4], led_array[4];
int button_press1 = 0, button_press2 = 0, button_press3 = 0, button_press4 = 0, send_rem;
short value, RNG_flag = -1, value2;

const unsigned int Mastermind_values[24][4] = {{1, 2, 3, 4}, {1, 2, 4, 3}, {1, 3, 4, 2}, {1, 3, 2, 4}, {1, 4, 2, 3}, {1, 4, 3, 2}, {2, 1, 3, 4}, {2, 1, 4, 3}, {2, 3, 4, 1}, {2, 3, 1, 4}, {2, 4, 1, 3}, {2, 4, 3, 1}, {3, 1, 2, 4}, {3, 1, 4, 2}, {3, 2, 4, 1}, {3, 2, 1, 4}, {3, 4, 1, 2}, {3, 4, 2, 1}, {4, 2, 3, 1}, {4, 2, 1, 3}, {4, 3, 1, 2}, {4, 3, 2, 1}, {4, 1, 2, 3}, {4, 1, 3, 2}};

int note1[7] = {440, 240, 880, 349, 394, 240, 0}; //notes played when player2 wins

void sr_init(void) //shfit register initialisation
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

void gpio_out(void) //Makes communication bit as output
{
    P3DIR |= BIT1;
    P3OUT |= BIT1;
}

void watchdog_init(void) //to restart the the game
{
    BCSCTL3 |= LFXT1S1;
    BCSCTL1 |= DIVA0;
    WDTCTL = WDTPW + WDTCNTCL + WDTSSEL;
}

void button_check()
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
        int i = 0;
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

void sr_clear(void)
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
    flag = -1;                //initialise flag=-1 (button=-1)
    WDTCTL = WDTPW + WDTHOLD; //Watchdog in hold
    P3DIR |= BIT0;
    P3OUT &= ~BIT0;
    P1DIR &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P1REN |= (BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P1OUT &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    sr_init();
    while (break_flag == 0)
    {
        if (P1IN & BIT3 || P1IN & BIT4 || P1IN & BIT5 || P1IN & BIT6 || P1IN & BIT7)
        {
            break_flag = 1;
        }
        else
            break_flag = 0;
    }

    decode();
    while (1)
    {
        while (turns < 3)
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
            gpio_out();

            if (correct_flag >= 4)
            {
                buzzer();
            }
            else
            {
                loser();
                __delay_cycles(1000000);
            }
            __delay_cycles(2000000);
            watchdog_init();
            while (1)
            {

            }
        }
    }
    return 0;
}

void buzzer() //Buzzer indicates win scenario
{
    int a = 0;
    P3DIR |= BIT6;
    P3SEL |= BIT6;
    P3REN &= ~BIT6;
    TA0CCTL2 = OUTMOD_3;
    for (a = 0; a < 7; a++)
    {
        TA0CCR0 = note1[a];       // PWM Period : 1000 us
        TA0CCR2 = note1[a] / 2;   // CCR2 PWM duty cycle (50 %)
        TA0CTL = TASSEL_2 + MC_1; // SMCLK ; MC_1 -> up mode ;
        __delay_cycles(250000);
    }
}

void loser() //Red LED glows
{
    P3DIR |= BIT2;
    P3OUT |= BIT2;
}

void check_array(void)
{
    int j;
    correct_flag = 0;
    for (j = 0; j < 4; j++)
    {
        if (guess_array[j] == Mastermind_values[RNG_flag][j])
        {
            led_array[j] = 1;
            correct_flag++;
        }
        else
            led_array[j] = 0;
    }
    if (correct_flag == 4)
    {
        turns = 2;
    }
}

void led_on(void) //switches on number of matches LEDs
{
    sr_init();
    P2OUT |= BIT6;
    sr_clear();
    for (led_state = 0; led_state < correct_flag; led_state++)
    {
        sr_clock();
    }
}

void decode(void) //decodes value of RNG_flag sent from board 1
{
    value = P1IN;
    value2 = value & 0xF0;
    RNG_flag = value2 / 8;
    if (P1IN & BIT3)
    {
        RNG_flag = RNG_flag + 1;
    }
}

void led_clear() //clears LEDs of shift register
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
