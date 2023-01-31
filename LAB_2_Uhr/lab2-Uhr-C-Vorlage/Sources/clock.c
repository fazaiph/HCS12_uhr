#include "clock.h"
#include "CA2Prototypes.h"
#include <hidef.h>                              // Common defines
#include <mc9s12dp256.h>                        // CPU specific defines
//#define SELECT12HOURS

// ****************************************************************************
// Global variables
extern char displayOutput[17];
int hours = 11;
int minutes = 59;
int seconds = 50;
char converted[6] ="";
unsigned char pressedButton = 0;
unsigned char stayInSetMode = 0;
int hoursForOutput = 0;


void incrHours(void)
{
    hours++;
    if (hours == 24) 
    {
        hours = 0;
    }
    pressedButton = 0;
}

void incrMinutes(void)
{
    minutes++;
    if (minutes == 60) 
    {
        minutes = 0;
        incrHours();
    }
    pressedButton = 0;
}

void incrSeconds(void) 
{
    seconds++;
    if (seconds==60) 
    {
        seconds = 0;
        incrMinutes();
    }
    pressedButton = 0;
}


void convertTodisplayOutputString(void)
{   
    #ifdef   SELECT12HOURS      //only relevant when in 12hour mode
    if(hours>=12 && hours !=0)  //if hours is greater than 12 we want pm and if it's also not 0(Midnight)
    {
      displayOutput[8] = 'p';
        
    } 
    else 
    {                           //else it is am
      displayOutput[8] = 'a';
    }
    displayOutput [9] = 'm';
    if (hours == 0 || hours == 12) 
    {
      hoursForOutput = 12;       //at midnight and at 12 in the middle of the day we want a 12 displayed instead of a 0
    } 
    else 
    {
      hoursForOutput = hours % 12;//use modulo 12 to get every hour over 12 as rest so hour modified hours ranges from 0-11 
    } 
    #else                       //not in 12hour mode we just want the displayOutput
    displayOutput[8] = ' ';     //not in 12hour mmode we want blankspace behind the displayOutput
    displayOutput[9] = ' ';
    hoursForOutput = hours;
    #endif
    // ****************************************************************************     HH:MM:SS
    //convert hours first                                                               01234567
    decToASCII_Wrapper(converted, hoursForOutput);
    displayOutput[0] = converted[4];
    displayOutput[1] = converted[5]; 
    
    // ****************************************************************************      
    //then convert minutes                                                               
    decToASCII_Wrapper(converted, minutes);
    displayOutput[3] = converted[4];
    displayOutput[4] = converted[5];
    

    // ****************************************************************************
    //last convert seconds
    decToASCII_Wrapper(converted, seconds);
    displayOutput[6] = converted[4];
    displayOutput[7] = converted[5];    
}

void displayClock(void)
{
    convertTodisplayOutputString();
    WriteLine_Wrapper(displayOutput, 1);
}

void tickClock(void) 
{
   incrSeconds();  
   displayClock();
}

void SetMode(void) 
{
    stayInSetMode = 1;
    asm MOVB #$08, PORTB;
    while(stayInSetMode==1) 
    {
      checkSetButtons();                    //check if any button is pressed
      if(pressedButton ==4)                 //SW3 is pressed
      {
        incrHours();
      }
      else{
      if(pressedButton ==2)                 //SW4 is pressed
      {
        incrMinutes();
      }else{
      if(pressedButton ==1)                 //SW5 is pressed
      {
        incrSeconds();
      }}}
      wait_10ms();  
      displayClock();                       //display time while setting
      checkButtonPressed();                 //check fo Button SW2
    }
    asm MOVB #$00, PORTB; 
}



