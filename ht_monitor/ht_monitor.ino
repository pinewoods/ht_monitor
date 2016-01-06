#include <EEPROM.h>
#include <DHT.h>
//#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <ESP8266.h>

#include "EEPROMAnything.h"
#include "ht_protocol.h"
#include "custom_esp.h"

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

  //ESP_SERIAL.listen();
  DEBUG_SERIAL.print("ESP8622: ");
  DEBUG_SERIAL.println(wifi.getVersion());

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

  // SET STATION
  bool station_error = wifi.setOprToStation();
  if(station_error==false){
    DEBUG_SERIAL.println("setOprToStation ERROR");
  }
  String aplist = wifi.getAPList();
  DEBUG_SERIAL.print("APLIST: ");
  DEBUG_SERIAL.println(aplist);


  // AP CONNECTION
  EEPROM_readAnything(0, configuration);
  bool joinap_error = wifi.joinAP(configuration.ssid, configuration.passwd);
  if(joinap_error==false){
    DEBUG_SERIAL.println("joinAP ERROR");
  }else{
    DEBUG_SERIAL.print("Connect to: ");
    DEBUG_SERIAL.print(configuration.ssid);
    DEBUG_SERIAL.println(wifi.getLocalIP());
  }

  // Disable IP MUX(single connection mode).
  wifi.disableMUX();
}

// If this was declared inside loop's scope, it causes crashes
char json_payload[64]="";
char tcp_payload[256]="";

void loop()
{

  digitalWrite(LED_BUILTIN, LOW);
  if(ht_state){
    wifi.leaveAP();
    htProtocolLoop();
  }

  //delay(dht.getMinimumSamplingPeriod());
  //delay(2000);
  delay(15000);

  dht_json_wrapper(json_payload, 64, dht.getHumidity(), dht.getTemperature());
  http_wrapper(tcp_payload, 256, json_payload, strlen(json_payload));

  DEBUG_SERIAL.print("strlen(tcp_payload): ");
  DEBUG_SERIAL.println(strlen(tcp_payload));
  DEBUG_SERIAL.println(tcp_payload);

  /*
  // SEND DATA
  bool tcp_error = wifi.createTCP ("192.168.0.134", 8000);
  // Create TCP connection in single mode.
  if(tcp_error==false){
    DEBUG_SERIAL.println("createTCP ERROR");
  }else{
    //bool send (const uint8_t *buffer, uint32_t len) : Send data based on TCP or UDP builded already in single mode. 
    //send (const uint8_t *buffer, uint32_t len);
    //bool releaseTCP (void) : Release TCP connection in single mode.
    wifi.releaseTCP();
  }
  */
}
