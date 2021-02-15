
//Colin Kerr January 2021 
//This version works on Wemos - change GPIO if different board.
//Compiled as LOLIN(WEMOS) D1 R2 & Mini.   ID photo here  https://desertbot.io/blog/d1-mini-blink

#include <ESP8266WiFi.h>    
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <ThingSpeak.h>          //https://github.com/mathworks/thingspeak-arduino
#include <WiFiClient.h>
WiFiClient client;
#include <RCSwitch.h>     //https://github.com/sui77/rc-switch
RCSwitch mySwitch = RCSwitch();

unsigned long myTempChannel = ***********;   //Thingspeak 
unsigned long myHumidChannel = ***********;  //
const char * myTempWriteAPIKey = "***********"; //Your Write API Key
const char * myHumidWriteAPIKey = "***********";

int iTemp = 0, iHumid = 0; 
unsigned long ulCurrentMillis = 0; 

void setup() {
  Serial.begin(9600);
  Serial.println("started ESP8266 WiFi Manager");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH); //start with it OFF (Pin HIGH)
  
  setUpWifi();  //Use WiFiManager to scan for WiFi signal and enable login.
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("waiting for connection");
  }
  Serial.println("IP address: ");
  Serial.println( WiFi.localIP());
  Serial.print("Connecting to ThingSpeak IO");
  ThingSpeak.begin(client);
  int val=0;
  int x = ThingSpeak.writeField(myTempChannel, 1, val, myTempWriteAPIKey); //Update in ThingSpeak
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else {Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  mySwitch.enableReceive(D5);  // Receiver pin D5 on Wemos
}

void loop() {
int x;
  iTemp = checkReceiver();
  if ( iTemp != -99 ) {
    x = ThingSpeak.writeField(myTempChannel, 1, iTemp, myTempWriteAPIKey); //Update in ThingSpeak
    if(x == 200){
      Serial.println("Temperature Channel update successful.");
    } else {
      Serial.println("Problem updating Temperature channel. HTTP error code " + String(x));
    }
    x = ThingSpeak.writeField(myHumidChannel, 1, iHumid, myHumidWriteAPIKey); //Update in ThingSpeak
    if(x == 200){
      Serial.println("Humid Channel update successful.");
    } else {
      Serial.println("Problem updating Humid channel. HTTP error code " + String(x));
    }
    ulCurrentMillis = millis();
    digitalWrite(LED_BUILTIN,LOW);
  }
  if (millis() - ulCurrentMillis > 30000 ) {
    digitalWrite(LED_BUILTIN,HIGH);
    ulCurrentMillis = millis();
  }
}

int checkReceiver() {
  int temp_data;
  iTemp = -99;
  if (mySwitch.available()) {
    output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    int protocol = mySwitch.getReceivedProtocol();
    unsigned long receivedData = mySwitch.getReceivedValue();
    int myDelay = mySwitch.getReceivedDelay();
    Serial.print("protocol = ");Serial.print(protocol);Serial.print("  Pulse Len = ");Serial.print( myDelay );Serial.print(" Data = ");Serial.println(receivedData);

    if  (protocol==6 && receivedData/10000 == 1234 ) {
      temp_data = receivedData - 12340000;
      iHumid = temp_data%100;
      iTemp = temp_data/100 - 40;
      Serial.print("iHumid = ");Serial.println(iHumid);
      Serial.print("iTemp = ");Serial.println(iTemp);
    }
    mySwitch.resetAvailable();   
  } 
  return iTemp;
}

void setUpWifi() {
   WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

   //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
   WiFiManager wm;

    //reset settings - wipe credentials for testing
    //wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
}
