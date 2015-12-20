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

  DEBUG_SERIAL.print("ESP8622: ");
  DEBUG_SERIAL.println(v);

  /*
  uint8_t mode = -1;
  bool station_mode = wifi.qATCWMODE(&mode);
  if(!station_mode){
    DEBUG_SERIAL.println("qATCWMODE ERROR");
  }else{
    DEBUG_SERIAL.print("setOprToStation: ");
    DEBUG_SERIAL.println(mode);
  }
  */

  bool station_error = wifi.setOprToStation();
  if(!station_error){
    DEBUG_SERIAL.println("setOprToStation ERROR");
  }
  String aplist = wifi.getAPList();
  DEBUG_SERIAL.print("APLIST: ");
  DEBUG_SERIAL.println(aplist);


  // AP CONNECTION
  EEPROM_readAnything(0, configuration);
  bool joinap_error = wifi.joinAP(configuration.ssid, configuration.passwd);
  if(!joinap_error){
    DEBUG_SERIAL.println("joinAP ERROR");
  }else{
    DEBUG_SERIAL.print("Connect to: ");
    DEBUG_SERIAL.print(configuration.ssid);
    DEBUG_SERIAL.println(wifi.getLocalIP());
  }
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  if(ht_state){
    wifi.leaveAP();
    htProtocolLoop();
  }

  //ESP_SERIAL.listen();
  String v = wifi.getVersion();

  //delay(dht.getMinimumSamplingPeriod());
  delay(2000);
  DEBUG_SERIAL.print("DHT22: ");
  DEBUG_SERIAL.print(dht.getHumidity());
  DEBUG_SERIAL.print(" ");
  DEBUG_SERIAL.println(dht.getTemperature());

}
