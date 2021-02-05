Using a remote battery powered Pro Mini with DHT22 (Humidity and Temperature sensor) with RF433 transmitter to send data to a mains powered Wemos (ESP8266) connected to the Internet for updating Thingspeak.


For battery operation the Pro Mini spends most of the time in Sleep Mode using the LowPower.h library. In addition the regulator and power LED has been removed reducing the quiescent current to around 5uA. 
The DHT22 requires nearly 2s warm up time so the Pro Mini sleeps during that period as well. 
The DHT22 and RF433 transmitter have unacceptable quiescent currents so each is powered by a GPIO pin when operational.
These steps reduce the average current for 30 minute readings to around 10uA giving a battery life of years from 3*AAAs.
The Pro Mini and sensors are good up to 5v and the DHT22 down to 3.3v. The transmitter will keep going until the Pro Mini is shut down by the brownout detector. It could go lower by turning off brownout detection but there is no point if the DHT22 is not operational.

The transmitter uses the RC-SWITCH library. To save power the default repeat transmissions and pulse length have been overriden. They can be increased if need be but an earlier project found reliable transmission on even lower settings. Messages do get lost in transmission but this seems to occur equally with higher settings.

The Pro Mini has an internal voltage sensor so in order to give warning of battery depletion the frequency of transmission reduces as the battery gets low. Alkaline are mostly empty at 1.1 volts at which point the DHT22 will cease to work. The frequency of messages reduces at 3.9v to every 60 mins and at 3.6v to every 90 mins. 

Messages are 24 bit numbers on protocol 6 in RC-Switch. Chosen as an earlier project found this worked best on low pulse and repeat.
On startup there is a test tranmission of 11223344 to help with testing on set up.
The two digit temperature and humidity values are appended to the digits 1234 but in order to allow for negative temperatures the temperature figure is offset by adding 40. 

The DHT22 is meant to have a pull up resistor as the Pro Mini ones are not strong enough. However this may matter only when a long connecting wire is used as it appears to be working fine without.

The GPIO pins powering the sensors are set low when they are not in use as the transmitter proved to have a 5mA quiescent current and the DHT22 was higher than acceptable. The DHT method dht.Begin() pulls the sensor pin high so that when it's powered off there's a current drawn so it's set low when the DHT is not being used. 


WEMOS uses the WiFiManager library.
On startup, it sets up in Station mode and tries to connect to a previously saved Access Point.
If this is unsuccessful (or no previous network saved) it moves the ESP into Access Point mode and spins up a DNS and WebServer (default ip 192.168.4.1)
Using any wifi enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point. 
Password is currently set to password (see setUpWifi() at tend of code.
Because of the Captive Portal and the DNS server you will either get a 'Join to network' type of popup or get any domain you try to access redirected to the configuration portal.
Choose one of the access points scanned, enter password, click save.
ESP will try to connect. If successful, it relinquishes control back to your app. If not, reconnect to AP and reconfigure.


