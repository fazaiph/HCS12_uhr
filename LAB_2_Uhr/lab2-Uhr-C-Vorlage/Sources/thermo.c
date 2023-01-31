#include "thermo.h"
int temp = 1023, tempOutput = 1023;        //modify temp to test since we don't simulate the potentiometer
char convertedTemp[6] ="";              //following vakues should give certain temperatures 0 = -30C, 1023 =70C
extern char displayOutput[17];          //the middle 511 roundabout 19C
int maxTemp = 70, minTemp = -30, adcResolution = 1023;

void ADtoTemp(void);

void convertTempToDisplayOutput(void)
{
   decToASCII_Wrapper(convertedTemp, tempOutput);
   displayOutput[11] = convertedTemp[0];
   displayOutput[12] = convertedTemp[4];
   displayOutput[13] = convertedTemp[5];
   if (tempOutput>-10 && tempOutput<10)    //when temperature is between -9 and 9 w don't want leading 0's
   {                                       //and move the - one char to the right
     displayOutput[11] = ' ';
     displayOutput[12] = convertedTemp[0];
   }
}

void thermoUpdate(void) 
{
  ADtoTemp();
  convertTempToDisplayOutput();
}



// - X X ° C
//1112131415