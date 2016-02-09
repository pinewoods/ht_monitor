#include <Wire.h>
#include <SoftwareSerial.h>
#include <ESP8266.h>
#include "rgb_lcd.h"

//#define ESP8266_USE_SOFTWARE_SERIAL
#define SERIAL_BUFFER_SIZE 256
#define TCP_BUFFER_SIZE 256

#define ESP_SERIAL Serial

//#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL cmdSerial
SoftwareSerial DEBUG_SERIAL(9, 8); /* RX:D9, TX:D8 */

ESP8266 wifi(ESP_SERIAL);
rgb_lcd lcd;

void setup()
{
  ESP_SERIAL.begin(115200);
  DEBUG_SERIAL.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(0, 128, 255);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  lcd.setCursor(0, 1);
  lcd.print("Nivel: ");
  delay(1000);

  // Set SoftAP parameters.
  // bool setSoftAPParam (String ssid, String pwd, uint8_t chl=7, uint8_t ecn=4)
  wifi.setSoftAPParam("ESP8266", "foobar");

  // Disable IP MUX(single connection mode).
  wifi.disableMUX();

  // SET SOFTAP
  bool station_error =  wifi.setOprToSoftAP();
  if(station_error==false){
    DEBUG_SERIAL.println("setOprToStation ERROR");
  }else{
    DEBUG_SERIAL.print("SUCCESS");
    // Create TCP connection in single mode.
    // createTCP (String addr, uint32_t port)
    wifi.createTCP ("192.168.0.1", 80);
  }
}

int nivel = 0;

void loop()
{

    lcd.setCursor(0, 1);
    lcd.print("Nivel: ");
    lcd.print(nivel);
    nivel++;

    //Get the IP list of devices connected to SoftAP.
    //String  getJoinedDeviceIP (void)
    String gjdi = wifi.getJoinedDeviceIP();
    DEBUG_SERIAL.println(gjdi);

    uint8_t buffer[TCP_BUFFER_SIZE];
    memset(buffer, 0, sizeof(uint8_t)*TCP_BUFFER_SIZE);

    // Receive data from TCP or UDP builded already in single mode.
    // uint32_t recv (uint8_t *buffer, uint32_t buffer_size, uint32_t timeout=1000)
    wifi.recv(buffer, TCP_BUFFER_SIZE);

    //DEBUG_SERIAL.println(buffer);
    //delay(1000);
}
