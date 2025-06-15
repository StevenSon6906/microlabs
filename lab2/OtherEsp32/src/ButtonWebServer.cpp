#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <LittleFS.h>
#include "AsyncStop.h"
#include"CurrentLed.h"
#include "WebServerBase.h"
#include "ButtonWebServer.h"

ButtonWebServer::ButtonWebServer(const char* ssid, const char* password, CommunicationService communicationService)
    : WebServerBase(80), wifiSSID(ssid), wifiPassword(password), communicationService(communicationService)
{
}

void ButtonWebServer::init()
{
    initAP(wifiSSID, wifiPassword);
    setupRoutes();
    server.on("/press", HTTP_POST, [this]() { this->handleButtonStatus(); });
    server.on("/pressRemote", HTTP_POST, [this]() { this->handleRemoteButtonStatus(); });
    server.on("/seq", HTTP_GET, [this]() { this->handleSeq();});
    start();
}
void ButtonWebServer::handleSeq()
{
    Serial.println("Seq requested");
    uint8_t currentLed = CurrentLed::getInstance().ledNumber;
    server.send(200, "text/plain", String(currentLed));
}

void ButtonWebServer::handleReactRouting()
{
    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/html", "<form action=\"/press\" method=\"post\"><button>Press me</button></form>");
    });
}

void ButtonWebServer::handleButtonStatus()
{
    Serial.println("Button status requested");
    AsyncStop::getInstance().request();
    server.send(200, "text/plain", "Button pressed");
}

void ButtonWebServer::handleRemoteButtonStatus()
{
    communicationService.send(ToogleCommand::ON);
}


