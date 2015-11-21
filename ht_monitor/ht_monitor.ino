#include <EEPROM.h>
#include <AltSoftSerial.h>
#include <DHT.h>
#include <ESP8266.h>

#include "EEPROMAnything.h"
#include "ht_protocol.h"


#define ESP8266_USE_SOFTWARE_SERIAL
#define SERIAL_BUFFER_SIZE 256

AltSoftSerial espSerial; /* RX:8  TX:9 Unusable:10 */
ESP8266 wifi(espSerial);
DHT dht;

void setup()
{
  // pinMode(10, INPUT);
  // pinMode(11, OUTPUT);
  // espSerial.begin(9600);

  dht.setup(2); // data pin 2
  htProtocolSetup();

  Serial.begin(115200);
  espSerial.begin(115200);
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  if(ht_state) htProtocolLoop();
  
  espSerial.listen();
  String v = wifi.getVersion();

  Serial.print("ESP8622: ");
  Serial.println(v);

  delay(dht.getMinimumSamplingPeriod());
  Serial.print("DHT22: ");
  Serial.print(dht.getHumidity());
  Serial.print(" ");
  Serial.println(dht.getTemperature());

}

