/* 
 * File:   newmain.c
 * Author: Mr Bryan
 * Lab Nine
 * Created on July 20, 2022, 1:44 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "LiquidCrystal.h"


# define _XTAL_FREQ 1000000
void __interrupt() adc_sample(void);

volatile int num;
float volt = 0.00;

int main() 
{
    
    // Configure LCD Pins
    // Data pins connected to PORTB
    TRISC = 0x00; 
    
    // RS = RD0
    // RW = RD1
    // E  = RD2
    TRISD = 0x00;
    
    // Output pins connected to seven segment
    TRISB = 0x00;
        
//    INTCON = 0x00;
    // connect the LCD pins to the appropriate PORT pins
    pin_setup(&PORTC, &PORTD);
    
    // initialize the LCD to be 16x2 (this is what I have, yours might be different)
    begin(16, 2, LCD_5x8DOTS);
    
    
    //--------------------------------------------------------------------------
    // 1 - Configure the A/D Module

    // * Configure analog pins, voltage reference and digital I/O 
    // AN0 is connected to wipe of potentiometer
    // Reference voltages are VSS and VDD
    ADCON1 = 0x0E;
    TRISAbits.RA0 = 1;

    // * Select A/D acquisition time
    // * Select A/D conversion clock
    // Right justified, ACQT = 2 TAD, ADCS = FOSC/2
    ADCON2bits.ADCS = 0; // FOSC/2
    ADCON2bits.ACQT = 1; // ACQT = 2 TAD
    ADCON2bits.ADFM = 1; // Right justified

    // * Select A/D input channel
    ADCON0bits.CHS = 0; // Channel 0 (AN0)

    // * Turn on A/D module
    ADCON0bits.ADON = 1;   
    
    // 2 - Configure A/D interrupt (if desired)
    // * Clear ADIF bit
    // * Set ADIE bit
    // * Select interrupt priority ADIP bit
    // * Set GIE bit
    
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    IPR1bits.ADIP = 1;
    RCONbits.IPEN = 0; // disable priority levels
    INTCONbits.PEIE = 1; // enable peripheral interrupts
    INTCONbits.GIE = 1;
    
    // 3 - Wait the required acquisition time (if required)
    // ---> using ACQT = 2 TAD, no need to manually specify a wait    
 
    while(1)
    {
        // 4- Start conversion: Set GO/DONE(bar) bit
        ADCON0bits.GO = 1;   
        
        // if you use clear() it will cause flicker
        // so instead pad with spaces then overwrite after going home
        // clear();
        // print_int(num);
        
        home();
        print("Raw: ");
        print_int(num);
        //print(":");
        //print_int(volt);          // finding the right voltage
        PORTB = volt;             // prints correctly on LCD but not on 7447
        print("       ");
        
        // 7 - Go to Step 1 or Step 2 as required                
    }    
    return 0;

}

void __interrupt() adc_sample(void)
{
    // test which interrupt called this interrupt service routine
    
    // ADC Interrupt
    if (PIR1bits.ADIF && PIE1bits.ADIE)
    {
        // 5 Wait for A/D conversion to complete by either
        // * Polling for the GO/Done bit to be cleared
        // * Waiting for the A/D interrupt
  
        // 6 - Read A/D result registers (ADRESH:ADRESL); clear bit ADIF, if required
        
        // reset the flag to avoid recursive interrupt calls
        PIR1bits.ADIF = 0;
        
        num = (ADRESH << 8) | ADRESL;
        // for the voltage calculation, I multiplied (num * 5 / 1024) by 10. I send each digit to a different 7447
        volt = (num * 50.00) / 1024.00;
    }
}
