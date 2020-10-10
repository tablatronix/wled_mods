#pragma once

#include "wled.h"
#include <NeoPixelBus.h>

    // how the hell do I put this in my class?
    RgbColor red(255, 0, 0);
    RgbColor green(0, 255, 0);
    RgbColor blue(0, 0, 255);
    RgbColor white(255);
    RgbColor black(0);
    RgbColor amber(255, 128, 0);

class neoIndicator : public Usermod {
  private:
    NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod>* neoInd = NULL;

    uint8_t  _rgbIndPin     = 0;     // pin to use, always uses bitbang
    bool     _rgbIndEnable  = true;  // used to disable , doubles as dim flag
    uint8_t  _indBright     = 100;   // indicator brightness

    bool     _dimmed        = false;  // dim flag
    uint16_t _dimDelay      = 60000; // millis to offBrightness/DIM
    uint16_t _dimBrightness = 5;     // dim timeout brightness , 0 for off

    uint16_t _wifiWait      = 15000; // millis to wait for wifi to connect
    bool     _startup       = true;  // startup flag


    // @todo replace timers with whatever is available from wled ( I assume it has timers )
    // timer vars
    // period
    int _timer0 = 300; // loop throttle
    int _timer1 = 300; // 
    int _timer2 = 0;   // blink slow
    int _timer3 = 0;   // blink fast

    // timestamps
    int _ts0 = 0;
    int _ts1 = 0;
    int _ts2 = 0;
    int _ts3 = 0;

    // bit state, simple toggle atm
    bool _timer0_EN = false;
    bool _timer1_EN = false;
    bool _timer2_EN = false;
    bool _timer3_EN = false;

  public:

    void setup() {
        if(_rgbIndPin > -1 && _rgbIndEnable){
            // Serial.println("neoIndicator: setup");
            neoInd = new NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod>(1, _rgbIndPin);
            neoInd->Begin();            
            neoInd->SetBrightness(_indBright);
            neoInd->Show(); // OFF
            delay(1);
            if(!_rgbIndEnable) return;  
            neoInd->SetPixelColor(0,amber); // startup
            neoInd->Show();

            // debugging LED_BUILTIN
            pinMode(2,OUTPUT);
            digitalWrite(2,HIGH);
        }
    }

    void loop() {
        if(!_rgbIndEnable) return;

        // throttle
        if(_ts0 && (millis() - _ts0 > _timer0)) return;
        _timer0 = millis();

        // apActive
        // Waiting for config / AP Mode
        if(!WLED_WIFI_CONFIGURED || apActive ){
            neoInd->SetPixelColor(0,blue);
            neoInd->Show();
        }
        // offline
        else if(!WLED_CONNECTED && !_startup){
            neoInd->SetPixelColor(0,red); // offline
            neoInd->Show();
        }

        // startup delay, prevents updates while showing wifi connect status
        if(millis() > 20000) _startup = false; // delay for wifi connect

        // dim/off timeout
        if((millis() > _dimDelay) && !_dimmed){
            _dimmed = true;
            if(_dimBrightness > 0) neoInd->SetBrightness(_dimBrightness);
            else neoInd->SetPixelColor(0,black); // off
            neoInd->Show();
        }
    }

    void connected() {
        if(!_rgbIndEnable) return;
        //Serial.println("Connected to WiFi!");
        _startup = false; // clear
        _dimmed  = false;
        neoInd->SetPixelColor(0,green); // connected
        neoInd->Show();
        digitalWrite(2,LOW); // LED_BUILTIN OFF
    }

    // void addToJsonState(JsonObject& root)
    // {
    //   //root["user0"] = userVar0;
    // }

    // void readFromJsonState(JsonObject& root)
    // {
    //   userVar0 = root["user0"] | userVar0; //if "user0" key exists in JSON, update, else keep old value
    //   //if (root["bri"] == 255) Serial.println(F("Don't burn down your garage!"));
    // }

    // uint16_t getId()
    // {
    //   return USERMOD_ID_EXAMPLE;
    // }

// indications ?
/*
Wifi good, bad
WLED_MQTT_CONNECTED
WLED_WIFI_CONFIGURED || apActive

Modes
Sleep - timing out
On - Off
Scheduled

Client Connected ( good for ID )
Pinging ?
*/

void blinkSlow(){
    // save color
    // check timer
    // turn off
    // turn back on
}

void blinkFast(){

}

void flash(uint8_t num,uint16_t delay){
    // set timer
    // will need state for blinking
    // alternative, use 1 timestamp, and calulate state from duration/period
}

};