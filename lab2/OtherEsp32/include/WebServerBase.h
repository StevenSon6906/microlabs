#ifndef WEBSERVERBASE_H
#define WEBSERVERBASE_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "AsyncStop.h"

class WebServerBase {
private:
    void handleReactorRouting();
public:
    WebServerBase(uint16_t port = 80);
    virtual ~WebServerBase();
    virtual void initAP(const char* ssid, const char* password, IPAddress ip = IPAddress(192, 168, 4, 1));
    virtual void setupRoutes();
    virtual void start();
    virtual void handleNotFound();
    void handleClient();
protected:
ESP8266WebServer server;
};

#endif
