#include <EEPROM.h>
#include <DHT.h>
//#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <ESP8266.h>

#include "EEPROMAnything.h"
#include "ht_protocol.h"


#define ESP8266_USE_SOFTWARE_SERIAL
#define SERIAL_BUFFER_SIZE 256

#define ESP_SERIAL Serial
//AltSoftSerial espSerial; /* RX:8  TX:9 Unusable:10 */

ESP8266 wifi(ESP_SERIAL);
DHT dht;

void setup()
{
  // pinMode(10, INPUT);
  // pinMode(11, OUTPUT);
  // espSerial.begin(9600);

  dht.setup(2); // data pin 2
  htProtocolSetup();

  ESP_SERIAL.begin(115200);
  DEBUG_SERIAL.begin(9600);
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  if(ht_state) htProtocolLoop();

  /*
  //ESP_SERIAL.listen();
  String v = wifi.getVersion();

  DEBUG_SERIAL.print("ESP8622: ");
  DEBUG_SERIAL.println(v);
  */

  //delay(dht.getMinimumSamplingPeriod());
  delay(2000);
  DEBUG_SERIAL.print("DHT22: ");
  DEBUG_SERIAL.print(dht.getHumidity());
  DEBUG_SERIAL.print(" ");
  DEBUG_SERIAL.println(dht.getTemperature());

}

String connect(){
  wifi.setOprToStation(); // Set operation mode to staion.
  wifi.joinAP (configuration.ssid, configuration.passwd); //Join in AP.
  return wifi.getLocalIP(); // Get the IP address of ESP8266.
  //leaveAP(); Leave AP joined before.
}
