#include <msp430.h> 
#include <templateEMP.h>

/***
 Name: Rayaprolu Venkata Sai Harsha
 Matriculation number: 4854097
 */

/**
 * main.c
 */

void ldr_init(void);
void ldr_main(void);
void adc_init(void);
void adc_main(void);
void sr_init(void);
void sr_clear(void);
void sr_clokc(void);
int i=0;
unsigned int m1[4];

void sr_clear(void)
{
    P2OUT &= ~ BIT5 ;
    P2OUT |= BIT5 ;

}

void sr_clock(void)
{
    P2OUT |= BIT4;
    P2OUT &=~BIT4;
}

void ldr_init(void)                 //Copied the same init as adc_init() from below
{
    ADC10CTL0 = ADC10ON + ADC10SHT_2 ;
    // Enable P1 .7 and P1.4 as AD input
    ADC10AE0 |= (BIT7 + BIT4);
    //  ADC10DTC1 = 4;                 // Four conversions
    // ADC10SA = (short)&ldr;       // ADC10 data transfer starting address.
    // Select channel 4 as input
    ADC10CTL1 = INCH_4 ;
}

void adc_init(void)
{
    ADC10CTL0 = ADC10ON + ADC10SHT_2 ;
    // Enable P1 .7 and P1.4 as AD input
    ADC10AE0 |= (BIT7 + BIT4);
    // ADC10DTC1 = 4;                     // Four conversions
    // ADC10SA = (short)&m1;          // ADC10 data transfer starting address.
    ADC10CTL1 = INCH_7 ;             // Select channel 7 as input for the following conversion (s)
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

int main(void)
{

    initMSP();                  //initialise microcontroller
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	adc_init();
	sr_init();
	//ldr_init();
	P3DIR  = BIT0 + BIT1 + BIT2 ;       //Initialise red, blue and green LED
	P3OUT &=~(BIT0+BIT1+BIT2);          //Clear red, blue and green LEDs
	//P1OUT |=BIT6;
	while (1)
	{
	       adc_main();
	  //     ldr_main();
	}
	return 0;
}
/***
     adc_main(): This is where the functioning of POT is seen
     Division 1: POT value from 1023-850 (All LEDs ON)
     Division 2: POT value from 850-650 (Only 3 LEDs ON)
     Division 3: POT value from 650-450 (Only 2 LEDs ON)
     Division 4: POT value from 450-250 (virtually till 200) (Only 1 LED ON)
     Division 5: POT value from 200-1 (In order to avoid overlap of division4 and division5) (All LEDs are off)
 */

void adc_main(void)
{
    int j=0;
    adc_init();
    //Start conversion
    ADC10CTL0 |= ENC + ADC10SC ;
    //Wait until result is ready
    while ( ADC10CTL1 & ADC10BUSY );
    //Copy conversion into m1
    int m1[] = ADC10MEM ;
    int a=m1[0];
    P2OUT |=BIT6;       // Initialise LEDs of Shift register


    if(a<1023 && a>=850)            //Division 1
    {

        sr_clear();         //clears shift register
        for(j=0;j<4;j++)
        {
            sr_clock();     //Gives four clock cycles
        }
    }
    else if(a < 850 && a >=650)     //Division 2
    {

        if(i==2)
        {
                sr_clear();         //clears Shift register
                // P2OUT &=~BIT6;
                for(j=0;j<3;j++)
                {
                    sr_clock();     //Gives three clock cycles
                }
            i++;
        }
        else
            i=2;

    }
    else if(a< 650 && a >= 450)   //Division 3
    {
       // P2OUT &= ~BIT6;
    //    sr_clear();
        if(i==1)
        {
                sr_clear();         //clears Shift register
                for(j=0;j<2;j++)
                {
                    sr_clock();     //Two clock cycles
                }

                i++;
        }
        else
            i=1;
    }
    else if(a< 450 && a >= 250)      //Division 4
    {
        //   sr_clear();
           if(i==0)
           {
               sr_clear();             //Clears Shift register
               sr_clock();             //Gives one clock cycle
               i++;
           }
           else
           i=0;
      //  sr_clear();
      //  P2OUT &= ~BIT6;

    }
    else if(a<200)              //Division 5
    {
        sr_clear();                 //clears Shift register
        P2OUT &= ~BIT6;
    }

    ldr_main();


   // __delay_cycles(1000000);
}
/***
ldr_main(): Functioning of LDR
            Contains arrays of all estimate values of the colours when LEDs..
            .. RED, GREEN, BLUE are toggled at 100msec each.
            When the ldr detects a value >50 and <280, it starts toggling the
            R, G, B LEDs.
            Detection of corresponding colours take place using the toggling LEDs.
            The colour is then printed onto the serial port(HTerm).
            Bonus: Yellow is detected as bonus colour

 */



void ldr_main(void)
{
    int red[]={137,138,139,140,141,142};
    int blue[]={180,181,182,183,184,185,186,189,190,191,192,193,194,195,196};
    int black[]={110,111,112,113,114,115,116,117,118,119,120,121,122,123,124};
    int green[]={128,127,129,130,126,131,132,130,133,125,126,127};
    int white[]={265,266,264,263,262,261,260,259};
    int yellow[]={143,144,145,146,147,148,149,150,152,153,154,155};
    ldr_init();
    //P1OUT |=BIT6;
    ADC10CTL0 |= ENC + ADC10SC ;
    //Wait until result is ready
    while ( ADC10CTL1 & ADC10BUSY );
    //Copy conversion into ldr
    int ldr[] = ADC10MEM ;
    int a2=ldr[0];
    int k=0;


    if((a2<280 && a2>50) || (a2<10))
    while((a2<280 && a2>50) || (a2<10) )
    {

       // serialPrintInt(a2);
        __delay_cycles(250000);
        P3OUT |= BIT1;              //green
        P3OUT =~(BIT0 + BIT2);
        __delay_cycles(100000);
        P3OUT |= BIT0;            //red
        P3OUT =~(BIT1 + BIT2);
        __delay_cycles(100000);
        P3OUT |= BIT2;              //blue
        P3OUT =~(BIT0 + BIT1);
        __delay_cycles(100000);
        for(k=0;k<=14;k++)
        if(a2==black[k])
        {

            serialPrint("BLACK detected\n");

        }
        for(k=0;k<=14;k++)
        if(a2==blue[k])
        {

            serialPrint("BLUE detected\n");
        }
        for(k=0;k<=5;k++)
        if(a2==red[k])
        {
            serialPrint("RED detected\n");

        }
        for(k=0;k<=7;k++)
        if(a2==white[k])
        {

            serialPrint("WHITE detected\n");
        }
        for(k=0;k<=11;k++)
        if(a2==green[k])
        {
            serialPrint("GREEN detected\n");
        }
        for(k=0;k<=11;k++)
        if(a2==yellow[k])
        {
            serialPrint("YELLOW detected\n");           //DHL delivery service yellow
        }
        a2=2000;

    }
    else                                            //added No colour detected(Slows down potentiometer)
    {
        serialPrint("NO COLOUR detected\n");
        __delay_cycles(300000);
    }
    if(a2>300 && a2!=2000)
    {
        P3OUT &=~(BIT0+BIT1+BIT2);
    }
   // flag=0;
 //   __delay_cycles(250000);
}





