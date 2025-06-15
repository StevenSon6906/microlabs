#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>
#include "CommunicationService.h"


#define BUTTON_DEBOUNCE_DELAY 200UL
#define LED_BLINK_INTERVAL 500UL
#define BUTTON_PIN 16U  
#define LED1_PIN 15U    
#define LED2_PIN 14U    
#define LED3_PIN 0U     

uint32_t ledOrder[] = {LED1_PIN, LED2_PIN, LED3_PIN}; 
uint32_t currentLed = 0;
bool buttonPressed = false;
uint32_t lastButtonPress = 0;
bool sequenceMode = true; 

SoftwareSerial mySerial(D7, D6, false);
CommunicationService communicationService(mySerial, 115200);
const char* ssid = "MyESP";
ESP8266WebServer server(80);

void processButtonAction() {
    if (sequenceMode) 
    {
        sequenceMode = false;
        Serial.println("Manual mode");
        return;
    }

    uint32_t nextLed = (currentLed + 1) % 3;
    if (digitalRead(ledOrder[currentLed]) == LOW) 
    {
        digitalWrite(ledOrder[currentLed], HIGH);
        digitalWrite(ledOrder[nextLed], HIGH);
        Serial.print("OFF LED: ");
        Serial.println(nextLed);
        return;
    } 
    digitalWrite(ledOrder[nextLed], LOW);
    Serial.print("ON LED: ");
    Serial.println(nextLed);
    currentLed = nextLed;    
}

void proceesRootRequest() {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
        server.send(500, "text/plain", "No index.html");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

void setup() {
    Serial.begin(112500);
    communicationService.init();

    pinMode(BUTTON_PIN, INPUT); 
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);

    for (int i = 0; i < 3; i++) 
    {
        digitalWrite(ledOrder[i], LOW);
    }

    if (!SPIFFS.begin()) {
        Serial.println("No init file system");
        return;
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    Serial.println("WiFi AP Started");

    server.on("/", HTTP_GET, proceesRootRequest);

    server.on("/press", HTTP_POST, []() 
    {
        processButtonAction();
        communicationService.send(ToogleCommand::ON);
        server.send(200, "text/plain", "Button press simulated.");
    });

    server.on("/pressRemote", HTTP_POST, []() 
    {
        communicationService.send(ToogleCommand::ON);
        server.send(200, "text/plain", "Button press simulated.");
    });

    server.on("/seq", HTTP_GET, []() {
        server.send(200, "text/plain", String(currentLed));
    });

    server.begin();
    Serial.println("Server started");

    server.on("/ledStatus", HTTP_GET, []() {
        String json = "{";
        json += "\"led1\":" + String(digitalRead(LED1_PIN) == HIGH) + ",";
        json += "\"led2\":" + String(digitalRead(LED2_PIN) == HIGH) + ",";
        json += "\"led3\":" + String(digitalRead(LED3_PIN) == HIGH);
        json += "}";
        server.send(200, "application/json", json);
    });
}

void handleButtonPress() {
    static uint32_t lastPressTime = 0;
    uint32_t now = millis();
    int buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == HIGH && !buttonPressed && (now - lastPressTime > BUTTON_DEBOUNCE_DELAY))
    {
        buttonPressed = true;
        lastPressTime = now;
        processButtonAction();
    }

    if (buttonState == LOW) 
    {
        buttonPressed = false;
    }
}

void handleLedSequence() {
    static uint32_t lastBlinkTime = 0;
    uint32_t now = millis();

    if (sequenceMode && (now - lastBlinkTime > LED_BLINK_INTERVAL)) 
    {
        lastBlinkTime = now;
        digitalWrite(ledOrder[currentLed], LOW);
        currentLed = (currentLed + 1) % (sizeof(ledOrder) / sizeof(ledOrder[0]));//in separet  var
        digitalWrite(ledOrder[currentLed], HIGH);
    }
}

void loop() {
    server.handleClient();
    communicationService.onReceive([](ToogleCommand command) {
        if (command == ToogleCommand::ON) {
            buttonPressed = true;
            processButtonAction();
    }});
    handleButtonPress();
    handleLedSequence();
}