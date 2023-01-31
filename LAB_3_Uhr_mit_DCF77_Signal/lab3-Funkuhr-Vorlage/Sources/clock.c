/*  Radio signal clock - Free running clock

    Computerarchitektur 3
    (C) 2018 J. Friedrich, W. Zimmermann Hochschule Esslingen

    Author:   W.Zimmermann, Jun  10, 2016
    Modified: -
*/

#include <stdio.h>

#include "clock.h"
#include "lcd.h"
#include "led.h"
#include "dcf77.h"

// Defines
#define ONESEC  (1000/10)                       // 10ms ticks per second
#define MSEC200 (200/10)

// Global variable holding the last clock event
CLOCKEVENT clockEvent = NOCLOCKEVENT;


// Modul internal global variables
static char hrs = 0, mins = 0, secs = 0;
static int uptime = 0;
static int ticks = 0;
static int timezone_curr = 0;     // EU -> 0; US -> 1

extern int timezoneChanged;
extern void checkButton(void);

// ****************************************************************************
//  Initialize clock module
//  Called once before using the module
void initClock(void)
{   displayTimeClock();
}

// ****************************************************************************
// This function is called periodically every 10ms by the ticker interrupt.
// Keep processing short in this function, run time must not exceed 10ms!
// Callback function, never called by user directly.
void tick10ms(void)
{   if (++ticks >= ONESEC)                      // Check if one second has elapsed
    {   clockEvent = SECONDTICK;                // ... if yes, set clock event
        ticks=0;
        toggleLED(0x01);                           // ... and turn on LED on port B.0 for every second
    }
    uptime = uptime + 10;                       // Update CPU time base

    dcf77Event = sampleSignalDCF77(uptime);     // Sample the DCF77 signal

        
    if(dcf77Event == VALIDSECOND || dcf77Event == VALIDMINUTE || dcf77Event == INVALID) {
      
      uptime = 0;        //uptime max 2 sec (uptime of a bit)  59 bits per transmission
    }
    
    checkButton();
    //--- End of user code
}

// ****************************************************************************
// Process the clock events
// This function is called every second and will update the internal time values.
// Parameter:   clock event, normally SECONDTICK
// Returns:     -
void processEventsClock(CLOCKEVENT event)
{   if (event==NOCLOCKEVENT)
        return;

    if (++secs >= 60)
    {   secs = 0;
        if (++mins >= 60)
        {   mins = 0;
            if (++hrs >= 24)
            {   hrs = 0;
            }
        }
     }
}

// ****************************************************************************
// Allow other modules, e.g. DCF77, so set the time
// Parameters:  hours, minutes, seconds as integers
// Returns:     -
void setClock(char hours, char minutes, char seconds)
{   hrs  = hours;
    mins = minutes;
    secs = seconds;
    
    if(timezoneChanged == 1) {         //Flag for change of timezone
    if(timezone_curr) {
      
      hrs = hrs -6;                    //calculation EU -> US
        if(hrs == -6){
          hrs = 18;
        }
        
        if(hrs == -5){
          hrs = 19;
        }
        
        if(hrs == -4){
          hrs = 20;
        }
        
        if(hrs == -3){
          hrs = 21;
        }
        
        if(hrs == -2){
          hrs = 22;
        }
        
        if(hrs == -1){
          hrs = 23;
        }
    }
     else {                           //calculation US -> EU
      
      hrs = hrs +6;
      
        if(hrs == 24){
          hrs = 0;
        }
        
        if(hrs == 25){
          hrs = 1;
        }
        
        if(hrs == 26){
          hrs = 2;
        }
        
        if(hrs == 27){
          hrs = 3;
        }
        
        if(hrs == 28){
          hrs = 4;
        }
        
        if(hrs == 29){
          hrs = 5;
        }
     
    }
    timezoneChanged = 0;
    }
   ticks = 0;
}

// ****************************************************************************
// Display the time derived from the clock module on the LCD display, line 0
// Parameter:   -
// Returns:     -
void displayTimeClock(void)
{   char uhrzeit[32] = "00:00:00 XX";
    
    if(timezone_curr == 1){
        (void) sprintf(uhrzeit, "%02d:%02d:%02d US", hrs, mins, secs );  //-> US Print
        writeLine(uhrzeit, 0);
    } 
    
    else{
        (void) sprintf(uhrzeit, "%02d:%02d:%02d DE", hrs, mins, secs );  //-> EU Print
        writeLine(uhrzeit, 0);
    }
    
    
}

// ***************************************************************************
// This function is called to get the CPU time base
// Parameters:  -
// Returns:     CPU time base in milliseconds
int time(void)
{   return uptime;
}

void settimeZone(int timezone) {
    timezone_curr = timezone;
    setClock(hrs, mins, secs);
}
