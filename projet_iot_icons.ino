/****Includes****/
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the &onfiguration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>          //Allow custom URL
#include "FastLED.h"


#define BRIGHTNESS  16
#define NUM_LEDS 64
#define DATA_PIN D6

CRGB leds[NUM_LEDS];

typedef enum color_e {SMILEY_CONTENT, SMILEY_MALHEUREUX,OFF} smiley;

/*****Initialization*****/
ESP8266WebServer server(80);
const char *ssid = "Livebox_7766";

/*****WebPage*****/
// Warning: only use simple quotes in the html (no double)
String rootHTML = "\
<!doctype html> <html> <head> <title> Choose your Smiley  </title> </head> <body>\
<br> <br> Analog input: xxx V (<a href='/'>refresh<a>)\
<form method='get' action='/set'>\
  <br><br> <button type='submit' name='toggle' value='1'>  :)  </button>\
  <br><br> <button type='submit' name='toggle' value='2'>  :(  </button>\
  <br><br> <button type='submit' name='toggle' value='0'>  off  </button>\
  <br><br> <button type='submit' name='toggle' value='3'>  dépassement  </button>\
</form>\
<br> No LED changed.\
</body> </html>\
";

String getHTML() {

    String updatedRootHTML = rootHTML;
    String voltage = String(analogRead(A0) * 3. / 1024.);
    updatedRootHTML.replace("xxx", voltage);
    return updatedRootHTML;
}

void handleRoot() {
    server.send(200, "text/html", getHTML());
}

void configModeCallback(WiFiManager *myWiFiManager) {
    //LEDfeedback(RED); // waiting for connection
}

void setupWifi() {
    //WiFiManager
    WiFiManager wifiManager;

    //reset saved settings -- Flush flash
    //wifiManager.resetSettings();

    //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect(ssid);

    // might seem redundant but it's not printed the 1st time:
    Serial.println("local ip");
    Serial.println(WiFi.localIP());
}

void setupServer() {
    server.on("/", handleRoot);
    server.on("/set", handleLEDs);
    server.begin();
    Serial.println("HTTP server started");
}
  
void setupMDNS() {
    // Add service to MDNS-SD to access the ESP with the URL http://<ssid>.local
    if (MDNS.begin(ssid)) {
        Serial.print("MDNS responder started as http://");
        Serial.print(ssid);
        Serial.println(".local");
    }
    MDNS.addService("http", "tcp", 8080);
}
void handleLEDs() {
    String smiley;
    if ( server.hasArg("toggle") ) {
        smiley = server.arg(0);
        Serial.println(smiley);
        LEDtoggle(smiley);
    } else {
        Serial.println("Bad URL.");
        server.send(404, "text/plain", "Bad URL.");
        return;
    }
    String answer = getHTML();
    answer.replace("No", smiley);
    server.send(200, "text/html", answer);
}

void LEDtoggle(String smiley) {
    int smiley_int = smiley.toInt();
    
    
    switch (smiley_int) {
        case 0 : set_blank();  Serial.print("OFF"); break;
        case 1 : smiley_happy() ; Serial.print("smiley happy") ; break;
        case 2 : smiley_not_happy() ; Serial.print("smiey non happy"); break;
        case 3 : arrow() ; Serial.print("arrow"); break;
         
        default:
            Serial.print("LEDtoggle() switch failed!");
            return;
    }
    //RGBstates[i] ^= 1; // toggle
    //analogWrite(RGBpins[i], RGBstates[i]*RGBintensities[i]);
}
void setup() {
  
  Serial.println("Starting WiFi.");

    Serial.begin(115200);
    setupWifi();
    setupServer();
    setupMDNS();

    delay(2000);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);

    set_blank();
    }

void loop(){
  server.handleClient();
  }

void set_blank(){
  Serial.print("Je suis dans le blanc");
  for(int i=0; i<64; i++){
    leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
 
}

void smiley_happy(){
    Serial.print("Je suis dans le smiley happy");
    set_blank();
    leds[10] = CRGB(255,0,0);
    leds[13] = CRGB(255,0,0);
    leds[18] = CRGB(255,0,0);
    leds[21] = CRGB(255,0,0);
    leds[41] = CRGB(255,0,0);
    leds[46] = CRGB(255,0,0);
    leds[50] = CRGB(255,0,0);
    leds[51] = CRGB(255,0,0);
    leds[52] = CRGB(255,0,0);
    leds[53] = CRGB(255,0,0);
    FastLED.show();
    delay(5000);
    set_blank();
    FastLED.show();
}

void smiley_not_happy(){
  Serial.print("Je suis dans le smiley non happy");
    set_blank();
    leds[10] = CRGB(0,0,255);
    leds[13] = CRGB(0,0,255);
    leds[18] = CRGB(0,0,255);
    leds[21] = CRGB(0,0,255);
    leds[49] = CRGB(0,0,255);
    leds[54] = CRGB(0,0,255);
    leds[42] = CRGB(0,0,255);
    leds[43] = CRGB(0,0,255);
    leds[44] = CRGB(0,0,255);
    leds[45] = CRGB(0,0,255);
    FastLED.show();
    delay(5000);
    set_blank();
    FastLED.show();
}

void arrow(){
    leds[11] = CRGB(0,0,255);
    leds[12] = CRGB(0,0,255);
    leds[18] = CRGB(0,0,255);
    leds[19] = CRGB(0,0,255);
    leds[20] = CRGB(0,0,255);
    leds[21] = CRGB(0,0,255);
    leds[25] = CRGB(0,0,255);
    leds[26] = CRGB(0,0,255);
    leds[27] = CRGB(0,0,255);
    leds[28] = CRGB(0,0,255);
    leds[29] = CRGB(0,0,255);
    leds[30] = CRGB(0,0,255);
    leds[35] = CRGB(0,0,255);
    leds[36] = CRGB(0,0,255);
    leds[43] = CRGB(0,0,255);
    leds[44] = CRGB(0,0,255);
    leds[51] = CRGB(0,0,255);
    leds[52] = CRGB(0,0,255);
    FastLED.show();
    moveImage(5000);
    set_blank();
    FastLED.show();
}

void moveImage(int delay_time) {
  for (int d = 0; d < delay_time/250; d ++) {
    CRGB new_leds[NUM_LEDS];
    delay(250); 
    for (int i = 0; i < NUM_LEDS; i++) {
      if (leds[i].r =! 0 or leds[i].g != 0 or leds[i].b != 0) {
        int new_i = i - 8;
        if (new_i < 0 ) {
          new_i = 64 + new_i;
        }
        new_leds[new_i] = CRGB(leds[i].r, leds[i].g, leds[i].b);
      }
    }
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = new_leds[i];
    }
    FastLED.show();
  }
}
