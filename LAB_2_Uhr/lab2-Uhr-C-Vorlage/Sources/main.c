/*  Lab 2 - Main C file for Clock program

    Computerarchitektur 3
    (C) 2018 J. Friedrich, W. Zimmermann
    Hochschule Esslingen

    Author:  W.Zimmermann, July 19, 2017
*/

#include <hidef.h>                              // Common defines
#include <mc9s12dp256.h>                        // CPU specific defines

#include "clock.h"
#include "CA2Prototypes.h"
#include "thermo.h"

#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"


// PLEASE NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:
// Files lcd.asm and ticker.asm do contain SOFTWARE BUGS. Please overwrite them
// with the lcd.asm file, which you bug fixed in lab 1, and with file ticker.asm
// which you bug fixed in prep task 2.1 of this lab 2.
//
// To use decToASCII you must insert file decToASCII from the first lab into
// this project
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ****************************************************************************
// Function prototype(s)
// Note: Only void Fcn(void) assembler functions can be called from C directly.
//       For non-void functions a C wrapper function is required.
void initTicker(void);

// Prototypes and wrapper functions for dec2ASCII (from lab 1)
void decToASCII(void);

void decToASCII_Wrapper(char *txt, int val)
{   asm
    {  	LDX txt
        LDD val
        JSR decToASCII
    }
} 
// Prototypes and wrapper functions for LCD driver (from lab 1)
void initLCD(void);
void writeLine(void);

void WriteLine_Wrapper(char *text, char line)
{   asm
    {	LDX  text
        LDAB line
        JSR  writeLine
    }
}

// ****************************************************************************

void initLED_C(void)
{   DDRJ_DDRJ1  = 1;	  	// Port J.1 as output
    PTIJ_PTIJ1  = 0;		
    DDRB        = 0xFF;		// Port B as output
    PORTB       = 0x55;
}

// ****************************************************************************
//more prototypes
void thermoUpdate(void);

// ****************************************************************************
// Global variables
unsigned char clockEvent = 0; 
unsigned char SETMODE = 0;
unsigned char secondsCopyrightUnchanged = 1;
unsigned char firstLineSelection = 0;
char displayOutput[17] = "HH:MM:SS   -XX°C";

// ****************************************************************************
void printCopyright(void)
{
  if(secondsCopyrightUnchanged > 9) 
        {
           secondsCopyrightUnchanged = 0;
           if (firstLineSelection == 0) 
           {
            firstLineSelection = 1;
           } else{
            firstLineSelection = 0;
           }
        }
        
        if (firstLineSelection==0) 
        { 
            WriteLine_Wrapper("Tetzlaff/Zaiser", 0);      //A line can show 16 characters
        } 
        else
        {
          WriteLine_Wrapper("(C) WS22/23", 0);
        }
}

void initButtons(void);
void initAD(void);

// ****************************************************************************
void main(void) 
{   EnableInterrupts;                           // Global interrupt enable
    

    initLED_C();                    		// Initialize the LEDs

    initLCD();                    		// Initialize the LCD
    //initAD();
    WriteLine_Wrapper("Tetzlaff/Zaiser", 0);
    displayClock();
    //WriteLine_Wrapper(timeToDisplay, 1);    

    initTicker();                              // Initialize the time ticker
    
    for(;;)                                    // Endless loop
    {   
      if (clockEvent)
      {   // ??? Add your code here ???
        printCopyright();
        tickClock();
        thermoUpdate();
        WriteLine_Wrapper(displayOutput, 1);   //output the displayOutput string modified by clock and thermometer
        secondsCopyrightUnchanged++;
        clockEvent = 0;
      }
      checkButtonPressed();                    //check for setmode button
      if(SETMODE==1) 
        {
          SetMode();  
        }
    }
}












