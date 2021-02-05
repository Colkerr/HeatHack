//Colin Kerr January 2021 
//Works in Pro Mini. Limit voltage to 5v.
//Check define for DHT11 or 22 as required
//DHT* needs 1 second to warm up before a reading if powered off
/*
  Based on the SendDemo example from the RC Switch library
  https://randomnerdtutorials.com/decode-and-send-433-mhz-rf-signals-with-arduino/
  https://github.com/sui77/rc-switch/
  DHT11/22
  https://www.instructables.com/Arduino-Temperature-and-Humidity-Sensor/
*/

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
#include <LowPower.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"  
#define DHTTYPE DHT22 
#define DHTPIN 2
#define DHTpowerPin 9
#define TransmitPowerPin 6
DHT dht(DHTPIN, DHTTYPE);
int iTemperature, iHumidity, iVoltage;

void setup() {
  Serial.begin(9600);
  Serial.println("Church Temp Humidity Sensor");
  delay(10);
  pinMode(DHTpowerPin, OUTPUT);  //LED for button press indication
  digitalWrite(DHTpowerPin, HIGH); 
  pinMode(TransmitPowerPin, OUTPUT);
  digitalWrite(TransmitPowerPin, LOW); // save power. low until has to power transmitter
  dht.begin();delay(2000);
  iTemperature = dht.readTemperature(); 
  iHumidity = dht.readHumidity(); 
  Serial.print("Temperature = ");Serial.println(iTemperature);
  Serial.print("Humidity = ");Serial.println(iHumidity);
  // Transmitter is connected to Arduino Pin #12
  mySwitch.setProtocol(6);
  mySwitch.setPulseLength(200); 
  mySwitch.enableTransmit(12);   //using pin 12
  transmit(11223344, 6, 24);  //test transmit                                             
}

void loop() {
   iVoltage = readVcc();
   pinMode(DHTpowerPin, OUTPUT); 
   digitalWrite(DHTpowerPin, HIGH); 
   dht.begin();
   LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);  //save energy while warming up. Needs at least 1.8s
   unsigned long ulData = readSensors();
   pinMode(DHTPIN, INPUT);  //set floating else it is high while DHT has low power rail
   digitalWrite(DHTpowerPin, LOW); 
   transmit(ulData, 6, 24);  
   //Serial.print("transmitted = ");Serial.println(ulData);
   //Serial.print("sec ");Serial.print(iSec);Serial.print(" min ");Serial.print(iMin);Serial.print(" hr ");Serial.print(iHr); ;Serial.print(" Mth ");Serial.println(iMnth);
   /* testing ##############################
       Serial.print("voltage = "); Serial.println(iVoltage);  delay(10);
       unsigned long ulVolt = 11300000  + iVoltage; 
       transmit(ulData, 6, 24); 
       delay(50);
    //end testing ##############################
  */
   sleep();  //sleep time depends on various factors.
}

long readVcc() {
  long result; //next two lines needed or after GetTemp() the voltage reads incorrectly.
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); delay(5);
  ADCSRA |= _BV(ADSC); // Convert while (bit_is_set(ADCSRA,ADSC));
  //above needed if temp measurement present.
  ADMUX = 0xC8; delay(10); // turn on internal reference, right-shift ADC buffer, ADC channel = internal temp sensor
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); delay(5);
  // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result;
  // Back-calculate AVcc in mV
  return result;
}


unsigned long readSensors() {   //and save to file
  unsigned long ulData; //value to return
  long lTempP50_100;
  //Serial.println("read the sensors");
  Serial.println(dht.readTemperature());
  Serial.println(dht.readHumidity());

  iTemperature = dht.readTemperature(); 
  iHumidity = dht.readHumidity(); 
  //Serial.print("Temperature = ");Serial.println(iTemperature);
  //Serial.print("Humidity = ");Serial.println(iHumidity);
  
  lTempP50_100 = ( 40 + iTemperature ) * 100;  //offset 40 to allow for below zero and shift x 100
  ulData = 12340000  + lTempP50_100 + iHumidity; 
  return ulData; 
}

void sleep() {    //signify low voltage by sending less frequently
//  sleepHere(1); //testing
   if ( iVoltage > 3900 ) {
     sleepHere(30);   //sleep 30 minutes     
   } else if ( iVoltage > 3600 ) { 
     sleepHere(60);   //sleep 60 minutes    
   } else {
     sleepHere(90); 
   }
}

//void transmit(unsigned long data, int repeats) {   //###
//  Serial.print(" transmit data "); Serial.println(data);
//}
void transmit(unsigned long data, int repeats, int bitLen) { //###
  digitalWrite(TransmitPowerPin, HIGH); // sets the digital pin on to power transmitter
  delay(5);
  mySwitch.setRepeatTransmit(repeats);
  mySwitch.send(data, bitLen); 
  digitalWrite(TransmitPowerPin, LOW); // save power
  //Serial.print(" transmitted data ");Serial.print(data); Serial.print("  bitlen  ");Serial.println(bitLen);
  //delay(10);    //##1 reduced from 500
}

void sleepHere(int iSleepMinutes)  {
  Serial.print("sleep "); Serial.println(iSleepMinutes);  delay(20);

  int iWakes = 60*iSleepMinutes/8 ;   //8 seconds is one sleep
  for (int w=0; w <= iWakes; w++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  Serial.print("  wake up ");
}
