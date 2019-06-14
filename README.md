# IOT vehicle icons

<p>
This project consists in building LED panels to display a chosen icon choosed on a web page. 
The aim would be to attach these boards to the front and rear of the vehicle so that we can inform other road users of our intentions. 

Thanks to that we can display on the panel if we want to:
<ul>
<li>Go beyond a user</li>
<li>Thank a user</li>
<li>Signify your annoyance</li>
<li>Ask a user to be careful</li>
<li>Accept a user's request</li>
<li>Refuse a user's request</li>
<li>Insult a user</li>
</ul>
This would make communication between users easier while avoiding ambiguities and possibly, many accidents. 
</p>

# Requirements
## Library
FastLED : https://github.com/FastLED/FastLED

## electronic material
<ul>
<li>1 Wemos d1 mini r2</li>
<li>3 Connectors</li>
<li> 1 LED Board CJMCU-8*8</li>
</ul>

# Installation

## Add external libraries
#### Install automaticaly
1. Open `Sketch` -> `Include Library` -> `Manage Libraries...`
2. Install `WiFiManager`

#### Install manually
1. Download the git file.zip
2. Open `Sketch` -> `Include Library` -> `Add .ZIP Library`

## Scheme
![Circuit](https://github.com/LarcherJulien/IOT_vehicle_icons/blob/master/scheme.PNG?raw=true "Title")

# Run
To run this project you need to open projet_iot_icons.ino with Arduino then 
<br/>change the SSID value to yours : `const char *ssid = "your ssid";`
<br/>If you run for the first time or wants to reset your wifi parametes you need to uncomment `wifiManager.resetSettings();`
<br/>Upload the code and Open logs
<br/>Connect to your SSID then open your navigator to change WIFI parameters to connect the iot to your access point.
<br/>Get the IP address in logs then connect to it via http
<br/>Enjoy !
  
## Authors

* **Charles-Maxime Gauriat**
* **Julien Larcher**
* **Charlotte Le Du**
