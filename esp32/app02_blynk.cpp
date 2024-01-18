#include <Arduino.h>
#include <esp_ota_ops.h>

#define BLYNK_TEMPLATE_ID "TMPL22O6KZbPX"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "Mrq1TQFt-opuppe0n6TZldwysCFM_FxY"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "x";
char pass[] = "x";

BlynkTimer timer;

const char* code_version = "esp_hst016l_v0.7_analog.cpp";
const char* device_num = "esp05";
#define LED 2

int decimalPrecision = 2;

int currentAnalogInputPin = 32;  // Use the appropriate analog pin on ESP32
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
float power;

unsigned long startMicrosSerial;
unsigned long currentMicrosSerial;
const unsigned long periodSerial = 1000000;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  int randomNumber = random(101);
  
  Blynk.virtualWrite(V2, randomNumber);
  power = FinalRMSCurrent * 240;
  Blynk.virtualWrite(V4, power);
  //Blynk.virtualWrite(V5, millis() / 1000);
}

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

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(LED, HIGH);
  //delay(500);
  //digitalWrite(LED, LOW);
  //delay(500);

  Blynk.run();
  timer.run();

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
