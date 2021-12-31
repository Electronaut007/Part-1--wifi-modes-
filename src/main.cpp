#include <Arduino.h>
#include<ESP8266WiFi.h>
#include<DNSServer.h>
#include<DHT.h>
#include<WiFiClient.h>
#include<ESP8266WebServer.h>
#include<Adafruit_Sensor.h>


// this is a test program
//comments are added to set nodeMCU in default AP mode to test
//this NodeMCU can run in both STA and AP modes on pushbutton command
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
<center>
<h1> LED CONTROL</h1><br>
Click to turn <a href="ledon">LED ON</a><br>
Click to turn <a href="ledoff">LED OFF</a><br>
CLick for DHT data <a href="dhtreadings"> DHT READING</a><br>
Click for Soil humidity sensor data <a href="soilsensor"> SOIL HUMIDITY SENSOR</a><br>
<hr>
</center>

</body>
</html>
)=====";



#define DHTTYPE DHT11
#define DHTPIN 4
int ledpin= LED_BUILTIN;
DHT dht(DHTPIN, DHTTYPE);
//#define pushbutton 1
int temp=0;
int lastbuttonstate= 0;
int analogdata;


ESP8266WebServer server(80);
WiFiClient Client;

char ssid[]="vineet";
char password[]="12345678";

char hotspotssid[]= "NodeMCUwebserver";
char hotspotpassword[]="123456789";

void handleRoot() {
 Serial.println("You called root page");
 String s = MAIN_page; 
 server.send(200, "text/html", s); 
}

void handleledon()
{
   Serial.println("LED on page");
 digitalWrite(ledpin,HIGH); 
 server.send(200, "text/html", "ON");
}

void handleledoff()
{
  Serial.println("LED off page");
  digitalWrite(ledpin,LOW);
  server.send(200,"text/html","OFF");
}

void handledhtreadings()
{
    double h = dht.readHumidity();
  double t = dht.readTemperature();
  Serial.println("DHT READINGS");
  server.send(200,"text/html",String(h));
  server.send(200,"text/html",String(t));

}

void handleanalogdata()
{
  Serial.println("SOil sensor data");
  server.send(200,"text/html",String(analogdata));

}

void stamode()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  Serial.println("connected to...");
  Serial.println(ssid);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //lastbuttonstate= 0;
   server.on("/",handleRoot);
  server.on("/ledon",handleledon);
  server.on("/ledoff", handleledoff);
  server.on("/dhtreadings",handledhtreadings);
  server.on("/soilsensor",handleanalogdata);
  server.begin();
}

void APmode()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(hotspotssid,hotspotpassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("switched to AP mode");
  Serial.println("IP Address...");
  Serial.println(myIP);
  //lastbuttonstate= 1;
  server.on("/",handleRoot);
  server.on("/ledon",handleledon);
  server.on("/ledoff", handleledoff);
 server.on("/dhtreadings",handledhtreadings);
 server.on("/soilsensor",handleanalogdata);
 server.begin();
  }


void setup()
{
  Serial.begin(9600);
  dht.begin();
  //pinMode(pushbutton, INPUT);
  pinMode(ledpin,OUTPUT);
  pinMode(DHTPIN,INPUT);
  pinMode(A0,INPUT);

  Serial.println("connecting to...");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  delay(1000);
  if(WiFi.status()!= WL_CONNECTED)
  {
    for (int i = 0; i <10 ; i++)
    {
      delay(500);
      Serial.println(".");
    }
  Serial.println("Unable to connect to wifi");
  Serial.println("use pusbutton to switch to AP mode");
  //temp=digitalRead(pushbutton);
  lastbuttonstate= 1;
  switch ((lastbuttonstate))
  {
  case 1: APmode();
    break;
  
  default: stamode();
    break;
  }
  
}
}

void loop()
{
analogdata= analogRead(A0);
  delay(1000);
  server.handleClient();
}

