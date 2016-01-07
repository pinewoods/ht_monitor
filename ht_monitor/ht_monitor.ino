#include <EEPROM.h>
#include <DHT.h>
//#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <ESP8266.h>

#include "EEPROMAnything.h"
#include "ht_protocol.h"
#include "simple_http.h"

#define ESP8266_USE_SOFTWARE_SERIAL
#define SERIAL_BUFFER_SIZE 256

#define ESP_SERIAL Serial
//AltSoftSerial espSerial; /* RX:8  TX:9 Unusable:10 */

ESP8266 wifi(ESP_SERIAL);
DHT dht;

bool pw_joinap(){
    EEPROM_readAnything(0, configuration);
    bool joinap_error = wifi.joinAP(configuration.ssid, configuration.passwd);
    if(joinap_error==false){
      DEBUG_SERIAL.println("joinAP ERROR");
    }else{
      DEBUG_SERIAL.print("Connected to: ");
      DEBUG_SERIAL.println(configuration.ssid);
      DEBUG_SERIAL.println(wifi.getLocalIP());
    }
    return joinap_error;
}

bool pw_leaveap(){
  bool leaveap_error = wifi.leaveAP();
  DEBUG_SERIAL.print("leaveap_error: ");
  DEBUG_SERIAL.println(leaveap_error);
  return leaveap_error;
}

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

  // SET STATION
  bool station_error = wifi.setOprToStation();
  if(station_error==false){
    DEBUG_SERIAL.println("setOprToStation ERROR");
  }else{
    String aplist = wifi.getAPList();
    DEBUG_SERIAL.print("APLIST: ");
    DEBUG_SERIAL.println(aplist);


    // Disable IP MUX(single connection mode).
    wifi.disableMUX();
  }
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  if(ht_state){
    wifi.leaveAP();
    htProtocolLoop();
  }

  char json_payload[64]="";
  char tcp_payload[HTTP_PAYLOAD_SIZE]="";

  dht_json_wrapper(json_payload, 64, dht.getHumidity(), dht.getTemperature());
  http_wrapper(tcp_payload, HTTP_PAYLOAD_SIZE, json_payload, strlen(json_payload));

  DEBUG_SERIAL.print("strlen(tcp_payload): ");
  DEBUG_SERIAL.println(strlen(tcp_payload));
  DEBUG_SERIAL.println(tcp_payload);

  // JOIN AP
  bool joinap_error = pw_joinap();
  if(joinap_error==false){
    pw_leaveap();
    return;
  }

  // SEND DATA
  bool tcp_error = wifi.createTCP ("192.168.0.134", 8000); // TODO: Parametric Endpoint
  // Create TCP connection in single mode.
  if(tcp_error==false){
    DEBUG_SERIAL.println("createTCP ERROR");
  }else{
    // Casting is needed
    //bool send (const uint8_t *buffer, uint32_t len) : Send data based on TCP or UDP builded already in single mode. 
    wifi.send ((uint8_t *)tcp_payload, (uint32_t) strlen(tcp_payload));
    //bool releaseTCP (void) : Release TCP connection in single mode.
    wifi.releaseTCP();
  }

  pw_leaveap();

  //delay(dht.getMinimumSamplingPeriod());
  //delay(2000);
  delay(15000);

}
