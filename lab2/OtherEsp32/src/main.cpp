#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LittleFS.h>
#include "AsyncStop.h"
#include "CommunicationService.h"
#include "ButtonWebServer.h"
#include "CurrentLEd.h"
#include "ToogleCommand.h"
#define BUTTON_PIN 4U
#define LED_1_PIN 21U
#define LED_2_PIN 19U
#define LED_3_PIN 18U
#define ButtonDebounceTime 250U
#define LedDelayTiem 500U
#define ButtonLongPressTime 2000U
const uint8_t leds[] = {LED_1_PIN, LED_2_PIN, LED_3_PIN}; 
const uint8_t numLeds = sizeof(leds) / sizeof(leds[0]);
bool buttonState;            
bool lastButtonState = HIGH;  
bool isButtonPressed = 0;
bool isButtonEnoughPressed = 0;
bool buttonToggle = 0;
uint32_t lastDebounceTime = 0;
uint32_t previousMillis = 0; 
uint32_t buttonPressStartTime = 0;
SoftwareSerial uartBridge(4,5); // RX, TX
CommunicationService communicationService(uartBridge, 115200);
ButtonWebServer buttonWebServer("ARSEN_ESP32", "123456789", communicationService);

void setup()
{

  communicationService.init();  
  Serial.begin(115200);
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 // Serial2.begin(115200, SERIAL_6E2,16,17);
 // uartBridge.begin(115200, SERIAL_6E2,16,17);
  buttonWebServer.init();
  pinMode(BUTTON_PIN, INPUT); 
  for (int i = 0; i < numLeds; i++) 
  {
    pinMode(leds[i], OUTPUT);
  }
}

void blinkLED()
{
  uint32_t currentMillis = millis();
  if (currentMillis - previousMillis >= LedDelayTiem) 
   {
    uint8_t currentLed  = CurrentLed::getInstance().ledNumber;
    digitalWrite(leds[currentLed], LOW);
    currentLed = (currentLed + 1) % numLeds;
    digitalWrite(leds[currentLed], HIGH);
    CurrentLed::getInstance().ledNumber = currentLed;
    previousMillis = currentMillis;
  }
}

int readDebouncedButton() {
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) 
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > ButtonDebounceTime)
  {
    if (reading != buttonState)
     {
      buttonState = reading;
    }
  }
  lastButtonState = reading;
  return reading;
}

bool onRelease()
{
  uint32_t currentMillis = millis();
  uint32_t buttonState  = readDebouncedButton();
  if (buttonState == LOW)
   {
    if (!isButtonPressed)
     {
      buttonPressStartTime = currentMillis;
      isButtonPressed = true;
      Serial.println("Button pressed");
    }
    else if (currentMillis - buttonPressStartTime > ButtonLongPressTime) 
    {
      Serial.println("Button too long pressed");
      isButtonEnoughPressed = true;
    }
    return false;
  }
  else
  {
    isButtonPressed = false;
  }

  if (isButtonEnoughPressed) 
  {
    isButtonPressed = false;
    isButtonEnoughPressed = false;
    return true;
  }
  
  return false;
}


void blinkLedWithAsyncStop()
{
  if (onRelease()) 
  {
    AsyncStop::getInstance().request();
    communicationService.send(ToogleCommand::ON);
  }

  if (AsyncStop::getInstance().isActive()) 
  {
    return; 
  }

  blinkLED();

}

void loop() {

  buttonWebServer.handleClient();
  blinkLedWithAsyncStop();  
  communicationService.onReceive([](ToogleCommand command) {
    Serial.println("Received command");
    if (command == ToogleCommand::ON) 
    {
      AsyncStop::getInstance().request();
      communicationService.send(ToogleCommand::SUCCESSFULLY_RECEIVED);
    } 
    if (command == ToogleCommand::SUCCESSFULLY_RECEIVED) 
    {
      AsyncStop::getInstance().request();
    }
});
// communicationService.send(ToogleCommand::OFF);

}

