/*  Radio signal clock - DCF77 Module

    Computerarchitektur 3
    (C) 2018 J. Friedrich, W. Zimmermann Hochschule Esslingen

    Author:   W.Zimmermann, Jun  10, 2016
    Modified: -
*/

/*
; A C H T U N G:  D I E S E  S O F T W A R E  I S T  U N V O L L S T Ä N D I G
; Dieses Modul enthält nur Funktionsrahmen, die von Ihnen ausprogrammiert werden
; sollen.
*/
#ifndef _HCS12_SERIALMON
    #ifndef SIMULATOR 
      #define SIMULATOR
    #endif
  #endif


#include <hidef.h>                                      // Common defines
#include <mc9s12dp256.h>                                // CPU specific defines
#include <stdio.h>

#include "dcf77.h"
#include "led.h"
#include "clock.h"
#include "lcd.h"


// Global variable holding the last DCF77 event
DCF77EVENT dcf77Event = NODCF77EVENT;

// Modul internal global variables
static int  dcf77Year=23, dcf77Month=1, dcf77Day=17, dcf77Weekday=1, dcf77Hour=0, dcf77Minute=0;       //dcf77 Date and time as integer values
static char* weekdayArray[7] = {"MON","TUE","WEN","THU","FRI","SAT","SUN"};
static int lastSignals[10] = 0;
int stateMachineEnabled = 0; 
int transmission[59];
int transmIndex = 0, index = 0, signal_averaged = 0;



extern void initializePortSim(void);			// Use instead of initializePort() for testing
extern char readPortSim(void);				// Use instead of readPort() for testing

// Prototypes of functions simulation DCF77 signals, when testing without
// a DCF77 radio signal receiver
void initializePortSim(void);                   // Use instead of initializePort() for testing
char readPortSim(void);                         // Use instead of readPort() for testing
void checkParity(int transmission[]);
int compareLastSignals(int lastSignals[]);

// ****************************************************************************
// Initalize the hardware port on which the DCF77 signal is connected as input
// Parameter:   -
// Returns:     -
void initializePort(void)
{

  DDRH = 0x00;
}

// ****************************************************************************
// Read the hardware port on which the DCF77 signal is connected as input
// Parameter:   -
// Returns:     0 if signal is Low, >0 if signal is High
char readPort(void)
{

    return PTH_PTH0;
}

// ****************************************************************************
//  Initialize DCF77 module
//  Called once before using the module
void initDCF77(void)
{   setClock((char) dcf77Hour, (char) dcf77Minute, 0);
    displayDateDcf77();
    #ifdef SIMULATOR
      initializePortSim();
    #else
      initializePort();
    #endif
}

// ****************************************************************************
// Display the date derived from the DCF77 signal on the LCD display, line 1
// Parameter:   -
// Returns:     -
void displayDateDcf77(void)
{   char datum[32];

    (void) sprintf(datum, "%s  %02d.%02d.%04d", weekdayArray[dcf77Weekday - 1], dcf77Day, dcf77Month, dcf77Year+2000);
    writeLine(datum, 1);
}

// ****************************************************************************
//  Read and evaluate DCF77 signal and detect events
//  Must be called by user every 10ms
//  Parameter:  Current CPU time base in milliseconds
//  Returns:    DCF77 event, i.e. second pulse, 0 or 1 data bit or minute marker
 DCF77EVENT sampleSignalDCF77(int currentTime)
{ 

DCF77EVENT event = NODCF77EVENT;

    int signal;
    static int oldSignal = 1;
    
    #ifdef SIMULATOR
      signal = readPortSim();
    #else
      signal = readPort();
    #endif
    
    
    
    lastSignals[index] = signal;
    index++;
    if (index == 10) {
     index = 0; 
    }
    signal_averaged = compareLastSignals(lastSignals);
      
      if (signal_averaged <= 3 || signal_averaged >= 7) 
      {
        if(signal_averaged <= 3) {
         signal = 0; 
        } else {
         signal = 1; 
        }
       if(signal != oldSignal) {
          oldSignal = signal;
       if (signal == 0) {
          
         clrLED(0x02);                                      //Led off if signal is high
         
         if(currentTime >= 900 && currentTime <= 1100) 
         {
           event = VALIDSECOND;                             //1s +- 100ms -> Valid second impulse (with tolerance)
         } 
         
         else if(currentTime >= 1900 && currentTime <= 2100)
         {
          event = VALIDMINUTE;                              //2s +- 100ms -> Valid minute impulse (with tolerance)
          stateMachineEnabled = 1;
         } 
         
         else {
          event = INVALID;                                  //else -> Tlow/ Tpulse outside window
          clrLED(0x08);                                     // LED on port B.3 off (no or wrong data)
         }
        }
         
         else 
         {
            setLED(0x02);                                   //setLed port B.1 if signal is Low
            
            if(currentTime >= 80 && currentTime <= 130)     //100ms +- 30ms Valid 0 Bit
            {
              event = VALIDZERO;
            } 
            
            else if(currentTime >= 170 && currentTime <= 230) //200ms +- 30ms Valid 1 Bit
            {
              event = VALIDONE;
            } 
            
            else 
            {
              event = INVALID;                               //else -> Tlow/Tpulse outside window
              clrLED(0x08);                                  // LED on port B.3 off (no or wrong data)
            }
          }
      } 
      }
      
      else
        {
          event = NODCF77EVENT;                             //Signal did not change since last polling
          clrLED(0x02);
        }
      
      return event;
}

// ****************************************************************************
// Process the DCF77 events
// Contains the DCF77 state machine
// Parameter:   Result of sampleSignalDCF77 as parameter
// Returns:     -
void processEventsDCF77(DCF77EVENT event)
{

  if(stateMachineEnabled == 1) {

    if (transmIndex <59){
  
      switch(event) {

        case VALIDSECOND: 
          break; 	
        
        case VALIDMINUTE:  
          break;            
        
        case VALIDZERO:       
          transmission[transmIndex]=0;
          transmIndex++;  	
          break;            
        
        case VALIDONE:        
          transmission[transmIndex]=1;
          transmIndex++;        
          break;            
    
  case INVALID:            //reset startSignal to block statemachine reset counter
    stateMachineEnabled=0;    
    transmIndex=0;        
    PORTB_BIT2 = 1;   
    PORTB_BIT3 = 0;   
    break;            
    
  default:              
    break;            
}
}
else {
  DecodeTransmission(transmission); 
  transmIndex = 0;
}
}
}
// ****************************************************************************
// Process the DecodeTransmission
// Contains the trainsmissionArray
// Parameter:   -
// Returns:     -

void DecodeTransmission(int transmissionArray[]) 
{
    dcf77Minute= transmissionArray[21] + transmissionArray[22] * 2 + transmissionArray[23] * 4 + transmissionArray[24] * 8 + transmissionArray[25] * 10 + transmissionArray[26] * 20 + transmissionArray[27] * 40;
    dcf77Hour= transmissionArray[29] + transmissionArray[30] * 2 + transmissionArray[31] * 4 + transmissionArray[32] * 8 + transmissionArray[33] * 10 + transmissionArray[34] * 20; 
    dcf77Day= transmissionArray[36] + transmissionArray[37] * 2 + transmissionArray[38] * 4 + transmissionArray[39] * 8 + transmissionArray[40] * 10 + transmissionArray[41] * 20;
    dcf77Weekday= transmissionArray[42] + transmissionArray[43] * 2 + transmissionArray[44] * 4;
    dcf77Month= transmissionArray[45] + transmissionArray[46] * 2 + transmissionArray[47] * 4 + transmissionArray[48] * 8 + transmissionArray[49] * 10;
    dcf77Year= transmissionArray[50] + transmissionArray[51] * 2 + transmissionArray[52] * 4 + transmissionArray[53] * 8 + transmissionArray[54] * 10 + transmissionArray[55] * 20 + transmissionArray[56] * 40 + transmissionArray[57] * 80;
    
    checkParity(transmission);
}



// ****************************************************************************
// Process the DecodeTransmission
// Contains the trainsmissionArray
// Parameter:   -
// Returns:     -


void checkParity(int transmission[]) {
      
      int oneCounter = 0;       //counts ones in received signal
      int failure = 0;          // 1 when failure has occured
      int i = 0;
      
      // check Bit 21-27 (minutes)
      for(i = 21;i<28;i++) {
        if (transmission[i]==1) {
          oneCounter++;
        }
      }
      
      if ((oneCounter % 2 == 0)&&(transmission[28]==1)) {
        failure = 1;
      }
      
      if ((oneCounter % 2 == 1)&&(transmission[28] == 0)){
        failure = 1;
      }
      
      oneCounter = 0;
    
      //Bit 29-34  (hours)
      for(i = 21;i<35;i++) {
        if (transmission[i]==1) {
          oneCounter++;
        }
      }
      
      if ((oneCounter % 2 == 0)&&(transmission[35]==1)) {
        failure = 1;
      }
       
      if ((oneCounter % 2 == 1)&&(transmission[35] == 0)){
        failure = 1;
      }
      
      oneCounter = 0;
      
       
      //Bit 36-57     (day)
      for(i = 21;i<58;i++) {
        if (transmission[i]==1) {
          oneCounter++;
        }
      }
      
      if ((oneCounter % 2 == 0)&&(transmission[58]==1)) {
        failure = 1;
      }
       
      if ((oneCounter % 2 == 1)&&(transmission[58] == 0)){
        failure = 1;
      }
      
      oneCounter = 0;
     
      // do hours and minutes have values in their range?
      if ((dcf77Hour < 0) || (dcf77Hour > 23)){
        failure = 1;
		  }
		
		  if ((dcf77Minute < 0) || (dcf77Minute > 59)){
        failure = 1;
		  }
		
		  if((transmission[20]==0) || (failure == 1)){           // output failure on LEDs when Bit 20 is low or failure has occured
  	    PORTB_BIT2=1;                                  
  	    PORTB_BIT3=0;                                 
		  } else {  	  	
  	      setClock((char) dcf77Hour, (char) dcf77Minute, 0);  // no failures occured overwrite clocktime
	        PORTB_BIT2 = 0;				                              // set LEDs to succesfull configuration
		      PORTB_BIT3 = 1;
 	      }
}


int compareLastSignals(int lastSignals[]) {
  int average = 0;
  int i = 0;
  for(i = 0; i<10;i++) {
     average += lastSignals[i];
  }
  return average;
}



