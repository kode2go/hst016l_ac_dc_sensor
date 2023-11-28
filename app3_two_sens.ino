/* 0- General */

int decimalPrecision = 2;                   // decimal places for all values shown in LED Display & Serial Monitor

/* 1- AC Current Measurement */

int currentAnalogInputPin = A1;             // Which pin to measure Current Value (A0 is reserved for LCD Display Shield Button function)
int calibrationPin = A2;                    // Which pin to calibrate offset middle value
float manualOffset = 0.25;                  // Key in value to manually offset the initial value
float mVperAmpValue = 62.5;                 // If using "Hall-Effect" Current Transformer, key in value using this formula: mVperAmp = maximum voltage range (in milli volt) / current rating of CT
                                            // For example, a 20A Hall-Effect Current Transformer rated at 20A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 20A = 31.25mV/A 
                                            // For example, a 50A Hall-Effect Current Transformer rated at 50A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 50A = 12.5 mV/A
float supplyVoltage = 5000;                 // Analog input pin maximum supply voltage, Arduino Uno or Mega is 5000mV while Arduino Nano or Node MCU is 3300mV
float offsetSampleRead = 0;                 /* to read the value of a sample for offset purpose later */
float currentSampleRead  = 0;               /* to read the value of a sample including currentOffset1 value*/
float currentLastSample  = 0;               /* to count time for each sample. Technically 1 milli second 1 sample is taken */
float currentSampleSum   = 0;               /* accumulation of sample readings */
float currentSampleCount = 0;               /* to count number of sample. */
float currentMean ;                         /* to calculate the average value from all samples, in analog values*/ 
float RMSCurrentMean ;                      /* square roof of currentMean, in analog values */   
float FinalRMSCurrent ;                     /* the final RMS current reading*/

int currentAnalogInputPin2 = A3;            // New pin to measure Current Value for the second sensor
int calibrationPin2 = A4;                   // New pin to calibrate offset middle value for the second sensor
float manualOffset2 = 0.25;                 // Key in value to manually offset the initial value for the second sensor
float offsetSampleRead2 = 0;                /* to read the value of a sample for offset purpose later for the second sensor*/
float currentSampleRead2  = 0;              /* to read the value of a sample including currentOffset1 value for the second sensor*/
float currentLastSample2  = 0;              /* to count time for each sample. Technically 1 milli second 1 sample is taken for the second sensor */
float currentSampleSum2   = 0;              /* accumulation of sample readings for the second sensor*/
float currentSampleCount2 = 0;              /* to count number of sample for the second sensor. */
float currentMean2 ;                        /* to calculate the average value from all samples, in analog values for the second sensor*/ 
float RMSCurrentMean2 ;                     /* square roof of currentMean, in analog values for the second sensor */   
float FinalRMSCurrent2 ;                    /* the final RMS current reading for the second sensor*/

/* 2 - LCD Display  */

#include<LiquidCrystal.h>                    /* Load the LiquidCrystal Library (by default already built-in with Arduino software)*/
LiquidCrystal LCD(8,9,4,5,6,7);              /* Creating the LiquidCrystal object named LCD. The pin may be varies based on LCD module that you use*/
unsigned long startMicrosLCD;                /* start counting time for LCD Display */
unsigned long currentMicrosLCD;              /* current counting time for LCD Display */
const unsigned long periodLCD = 1000000;     // refresh every X seconds (in seconds) in LED Display. Default 1000000 = 1 second 
unsigned long previousMillis = 0;
const long interval = 1000;  // Print every 1000 milliseconds (1 second)

void setup()                                /*codes to run once */
{                                      
    /* 0- General */
    Serial.begin(9600);                      /* to display readings in Serial Monitor at 9600 baud rates */

    /* 2 - LCD Display  */
    startMicrosLCD = micros();               /* Start counting time for LCD display*/
}

void loop()                                 /*codes to run again and again */
{                                      
    /* 1- AC & DC Current Measurement */

    if(micros() >= currentLastSample + 200) /* every 0.2 milli second taking 1 reading */
    { 
        currentSampleRead = analogRead(currentAnalogInputPin)-analogRead(calibrationPin); 
        /* read the sample value including offset value for the first sensor */
        currentSampleRead2 = analogRead(currentAnalogInputPin2)-analogRead(calibrationPin2); 
        /* read the sample value including offset value for the second sensor */

        currentSampleSum = currentSampleSum + sq(currentSampleRead);
        /* accumulate total analog values for each sample readings for the first sensor */
        currentSampleSum2 = currentSampleSum2 + sq(currentSampleRead2); 
        /* accumulate total analog values for each sample readings for the second sensor */

        currentSampleCount = currentSampleCount + 1;
        currentSampleCount2 = currentSampleCount2 + 1;
        /* to count and move on to the next following count for both sensors */  

        currentLastSample = micros();
        currentLastSample2 = micros(); 
        /* to reset the time again so that next cycle can start again*/ 
    }

    if(currentSampleCount == 4000 && currentSampleCount2 == 4000) 
    /* after 4000 count or 800 milli seconds (0.8 second), do this following codes*/
    { 
        currentMean = currentSampleSum/currentSampleCount; 
        /* average accumulated analog values for the first sensor */
        currentMean2 = currentSampleSum2/currentSampleCount2; 
        /* average accumulated analog values for the second sensor */

        RMSCurrentMean = sqrt(currentMean); 
        /* square root of the average value for the first sensor */
        RMSCurrentMean2 = sqrt(currentMean2); 
        /* square root of the average value for the second sensor */

        FinalRMSCurrent = (((RMSCurrentMean /1023) *supplyVoltage) /mVperAmpValue)- manualOffset; 
        /* calculate the final RMS current for the first sensor */
        FinalRMSCurrent2 = (((RMSCurrentMean2 /1023) *supplyVoltage) /mVperAmpValue)- manualOffset2; 
        /* calculate the final RMS current for the second sensor */

        if(FinalRMSCurrent <= (625/mVperAmpValue/
