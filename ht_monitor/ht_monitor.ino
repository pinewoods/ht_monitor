#include <EEPROM.h>
#include <SoftwareSerial.h>

#include "DHT.h"
#include "EEPROMAnything.h"
#include "ht_protocol.h"
#include "ESP8266.h"

#define ESP8266_USE_SOFTWARE_SERIAL
#define SERIAL_BUFFER_SIZE 256
SoftSerial espSerial(10,  11); /* RX:D10, TX:D11 */
ESP8266 wifi(espSerial);
DHT dht;

void setup()
{
  // pinMode(10, INPUT);
  // pinMode(11, OUTPUT);
  espSerial.begin(9600);

  dht.setup(2); // data pin 2
  htProtocolSetup();
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  if(ht_state) htProtocolLoop();

  Serial.println(wifi.getVersion());
  espSerial.print("AT+RST");

  
  delay(dht.getMinimumSamplingPeriod());
  Serial.print(dht.getHumidity());
  Serial.print(" ");
  Serial.println(dht.getTemperature());
  
}

