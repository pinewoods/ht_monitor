#include <EEPROM.h>
#include "DHT.h"
#include "EEPROMAnything.h"
#include "ht_protocol.h"

DHT dht;

void setup()
{

  dht.setup(2); // data pin 2
  htProtocolSetup();
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  if(ht_state) htProtocolLoop();
  
  delay(dht.getMinimumSamplingPeriod());
  Serial.print(dht.getHumidity());
  Serial.print(" ");
  Serial.println(dht.getTemperature());
  
}

