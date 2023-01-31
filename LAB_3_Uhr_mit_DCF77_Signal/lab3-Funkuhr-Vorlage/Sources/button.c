#ifndef _HCS12_SERIALMON
    #ifndef SIMULATOR 
      #define SIMULATOR
    #endif
  #endif

#include <stdio.h>
#include <hidef.h>                              // Common defines
#include <mc9s12dp256.h>                        // CPU specific defines

// Defines
#define buttonPressed  20                       // 20x10ms button is pressed

  
  
#ifdef SIMULATOR                                //depending on hardware or simulation the signal is inverted
  #define buttonStatus PTH_PTH3
#else
  #define buttonStatus !PTH_PTH3
#endif

extern void setTimeZone(int);
void buttonAction(void);

// Modul internal global variables


static int timezone = 0;
int timezoneChanged = 0, pressedCounter = 0;

// ****************************************************************************
//  Initialize clock module
//  Called once before using the module
void checkButton(void)
{   
  if(buttonStatus) 
  {                                              //if button is pressed increment the counter
     pressedCounter++;         
  } 
  else 
  {
    pressedCounter = 0;
    return;
  }
  
  if(pressedCounter >= buttonPressed)           //until the counter hits 20. 20x10ms = 200ms means button was really pressed
  {
    buttonAction();
    pressedCounter = 0;
  }
    
     
}

void buttonAction(void) 
{
     if (timezone){
                  timezone = 0;             //EU
              } 
              else{
                  timezone = 1;             //US
              }
              timezoneChanged = 1;
              settimeZone(timezone);
}
  

              
             
            
