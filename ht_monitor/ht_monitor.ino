#include <EEPROM.h>
#include "DHT.h"
#include "EEPROMAnything.h"
#include "ht_protocol.h"

DHT dht;

void setup()
{

  dht.setup(2); // data pin 2
  ht_protocol_setup();
}

void loop()
{
  if(ht_state) ht_protocol_loop();
  
  delay(dht.getMinimumSamplingPeriod());
  Serial.print(dht.getHumidity());
  Serial.print(" ");
  Serial.println(dht.getTemperature());
  
}

