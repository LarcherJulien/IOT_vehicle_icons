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
int dataValues[] = {0, 0, 0, 0, 0};

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
  <br><br> <button type='submit' name='toggle' value='3'>  Depassement  </button>\
  <br><br> <button type='submit' name='toggle' value='4'>  Attention  </button>\
</form>\
<br> <br> <span>Content : <b id=\"content\">value<b/></span> <br> <br>\
<br> <br> <span>Pas content : <b id=\"pasContent\">value<b/></span> <br> <br>\
<br> <br> <span>Depassement : <b id=\"depassement\">value<b/></span> <br> <br>\
<br> <br> <span>Depassement : <b id=\"attention\">value<b/></span> <br> <br>\
</body>\
<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>\
<script>setInterval( () => { $.ajax('/data', { success: (d, ts, j) => { $('#content').html(d.data[0]); $('#pasContent').html(d.data[1]) ; $('#depassement').html(d.data[2]) ; $('#attention').html(d.data[3]) } }) }, 900);</script>\
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

void handleGetData() {
  String answer = "{\"data\": [" + String(dataValues[1]) + ", " + String(dataValues[2]) + ", " + String(dataValues[3]) + ", " + String(dataValues[4]) + "]}";
  server.send(200, "application/json", answer);
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
    server.on("/data", handleGetData);
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
        case 0 : set_blank();  Serial.println("OFF"); dataValues[0]++;  break;
        case 1 : smiley_happy() ; Serial.println("smiley happy") ; dataValues[1]++; break;
        case 2 : smiley_not_happy() ; Serial.println("smiley non happy"); dataValues[2]++;  break;
        case 3 : arrow() ; Serial.println("arrow"); dataValues[3]++; break;
        case 4 : attention() ; Serial.println("attention"); dataValues[4]++;  break;
         
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

void breath(int delayed){
  for(int i=0; i <100 ; i++){
    FastLED.setBrightness(i);
    FastLED.show();
    delay(delayed);
  }
    for(int i=100; i>0 ; i--){
    FastLED.setBrightness(i);
    FastLED.show();
    delay(delayed);
  }
}

void set_blank(){
  FastLED.setBrightness(16);
  Serial.print("Je suis dans le blanc");
  for(int i=0; i<64; i++){
    leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
 
}

void smiley_happy(){
    Serial.print("Je suis dans le smiley happy");
    set_blank();
    int num_leds[] = {10, 13, 18, 21, 41, 46, 50, 51, 52, 53};
    displayImage(num_leds, 10, 0xFF0000);
    FastLED.show();
    delay(5000);
    set_blank();
    FastLED.show();
}

void smiley_not_happy(){
  Serial.print("Je suis dans le smiley non happy");
    set_blank();
    int num_leds[] = {10, 13, 18, 21, 49, 54, 42, 43, 44, 45};
    displayImage(num_leds, 10, 0x00FF00);
    FastLED.show();
    delay(5000);
    set_blank();
    FastLED.show();
}

void arrow(){
    set_blank();
    int num_leds[] = {11, 12, 18, 19, 20, 21, 25, 26, 27, 28, 29, 30, 35, 36, 43, 44, 51, 52};
    displayImage(num_leds, 18, 0x0000FF);
    FastLED.show();
    moveImage(5000);
    set_blank();
    FastLED.show();
}


void attention(){
    int num_leds[] = {3,4,11,12,19,20,27,28,35,36,51,52,59,60};
    displayImage(num_leds, 14, 0x00FF00);
    for(int i = 0; i<10 ; i++){
      breath(5); 
    }
    set_blank();
}

void displayImage(int num_leds[], int size_leds, int color) {
  for (int i = 0; i < size_leds; i++) {
    leds[num_leds[i]] = color;
  }
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
