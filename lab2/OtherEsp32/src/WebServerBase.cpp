#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "AsyncStop.h"
#include "FS.h"
#include "LittleFS.h"
#include "WebServerBase.h"

WebServerBase::WebServerBase(uint16_t port)
    : server(port)
{
}

WebServerBase::~WebServerBase()
{
}

void WebServerBase::initAP(const char* ssid, const char* password, IPAddress ip)
{
    IPAddress subnet(255, 255, 255, 0);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAPConfig(ip, ip, subnet))
    {
        Serial.println("AP Config failed");
    }
    if (WiFi.softAP(ssid, password))
    {
        Serial.println("Access Point Started");
        Serial.print("AP SSID: ");
        Serial.println(ssid);
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    }
    else
    {
        Serial.println("AP Failed to start");
    }
}

void WebServerBase::setupRoutes()
{
    server.serveStatic("/", LittleFS, "/index.html");
    server.onNotFound([this]() { this->handleNotFound(); });
}

void WebServerBase::start()
{
    server.begin();
    Serial.println("HTTP server started");
}


void WebServerBase::handleNotFound()
{
    server.send(404, "text/plain", "404: Path Not Found");
}

void WebServerBase::handleClient()
{
    server.handleClient();
}
