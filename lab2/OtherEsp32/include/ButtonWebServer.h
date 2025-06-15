#ifndef BUTTONWEBSERVER_H
#define BUTTONWEBSERVER_H
#include "CommunicationService.h"
#include "WebServerBase.h"

class ButtonWebServer : public WebServerBase {
public:
    ButtonWebServer(const char* ssid, const char* password, CommunicationService communicationService);
    virtual void init();
    virtual void handleButtonStatus();
    virtual void handleSeq();
    void handleRemoteButtonStatus();
private:
    const char* wifiSSID;
    const char* wifiPassword;
    CommunicationService communicationService;
    void handleReactRouting();
};

#endif
