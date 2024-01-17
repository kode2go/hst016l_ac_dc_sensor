#include <Arduino.h>
const char* code_version = "esp_hst016l_v0.6_analog.cpp";
const char* device_num = "esp05";
#define LED 2

int decimalPrecision = 2;

int currentAnalogInputPin = 34;  // Use the appropriate analog pin on ESP32
int calibrationPin = 35;
float manualOffset = 2.00;
float mVperAmpValue = 12.5;
float supplyVoltage = 3300;  // ESP32 operates at 3.3V
float offsetSampleRead = 0;
float currentSampleRead = 0;
float currentLastSample = 0;
float currentSampleSum = 0;
float currentSampleCount = 0;
float currentMean;
float RMSCurrentMean;
float FinalRMSCurrent;

unsigned long startMicrosSerial;
unsigned long currentMicrosSerial;
const unsigned long periodSerial = 1000000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  //pinMode(13, OUTPUT);

  Serial.println("*****************************************************");
  Serial.println("Code version: ");
  Serial.println(code_version);
  Serial.println("Device Number: ");
  Serial.println(device_num);

  startMicrosSerial = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(LED, HIGH);
  //delay(500);
  //digitalWrite(LED, LOW);
  //delay(500);

  if (micros() >= currentLastSample + 200) {
    currentSampleRead = analogRead(currentAnalogInputPin) - analogRead(calibrationPin);
    currentSampleSum = currentSampleSum + sq(currentSampleRead);
    currentSampleCount = currentSampleCount + 1;
    currentLastSample = micros();
  }

  // Serial.print("The Current currentLastSample is: ");
  // Serial.println(currentSampleCount);

  if (currentSampleCount == 4000) {
    currentMean = currentSampleSum / currentSampleCount;
    RMSCurrentMean = sqrt(currentMean);
    FinalRMSCurrent = (((RMSCurrentMean / 4095) * supplyVoltage) / mVperAmpValue) - manualOffset;

    if (FinalRMSCurrent <= (625 / mVperAmpValue / 100)) {
      FinalRMSCurrent = 0;
    }

    Serial.print("The Current RMS value is: ");
    Serial.print(FinalRMSCurrent, decimalPrecision);
    Serial.println(" A");

    currentSampleSum = 0;
    currentSampleCount = 0;
  }

  currentMicrosSerial = micros();
  if (currentMicrosSerial - startMicrosSerial >= periodSerial) {
    startMicrosSerial = currentMicrosSerial;
  }

}
