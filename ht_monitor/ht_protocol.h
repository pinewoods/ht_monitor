#include <avr/interrupt.h>
#include "is_double.h"

#define SSID_SIZE 32
#define PASSWD_SIZE 32
#define E2PROM_DEBUG

#define RX_BUFFER_SIZE 64
#define RX_TIMEOUT_MS 1000

#define OK_RESPONSE 20
#define NO_RESPONSE 23
#define NOT_FOUND_RESPONSE 44
#define ERROR_RESPONSE 50

/*
ISR_ADDR reference for AtMega328
--------------------------------------------
attachInterrupt - Name - Pin on chip - Pin on board
0                         - INIT0  - 4                   - D2
1                         - INIT1  - 5                   - D3
*/
#define ISR_ADDR 1

volatile char ht_state = LOW;

#define SERIAL Serial
// #define SERIAL cmdSerial
// SoftwareSerial cmdSerial(10,  11); /* RX:D10, TX:D11 */

struct config_t
{
    // User defined data
    float target_min_temp;
    float target_temp_max;
    // Runtime defined data
    float min_temp;
    float temp_max;
    // Wifi connection data
    char ssid[SSID_SIZE];
    char passwd[PASSWD_SIZE];
} configuration;

void htProtocolSetup();
void htProtocolTrigger();
void htProtocolLoop();

void htProtocolSetup(){
  // SERIAL.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  EEPROM_readAnything(0, configuration);

  attachInterrupt(ISR_ADDR, htProtocolTrigger, RISING);
    
  #ifdef E2PROM_DEBUG 
    SERIAL.print("TARGET_MIN_TEMP: ");
    SERIAL.println(configuration.target_min_temp); 
    SERIAL.print("TARGET_MAX_TEMP: ");
    SERIAL.println(configuration.target_temp_max); 
    SERIAL.print("MIN_TEMP: ");
    SERIAL.println(configuration.min_temp); 
    SERIAL.print("TEMP_MAX: ");
    SERIAL.println(configuration.temp_max); 
    SERIAL.print("SSID: ");
    SERIAL.println(configuration.ssid); 
    SERIAL.print("PASSWD: ");
    SERIAL.println(configuration.passwd); 
  #endif
}

void htProtocolTrigger(){
   detachInterrupt(ISR_ADDR);
   ht_state = HIGH;
   digitalWrite(LED_BUILTIN, HIGH);
}

void htProtocolLoop(){

    char buffer[RX_BUFFER_SIZE] = "";
    //memset(buffer,0,RX_BUFFER_SIZE);
    SERIAL.setTimeout(RX_TIMEOUT_MS);

    float fvalue;
    char svalue[RX_BUFFER_SIZE] = "";
    memset(svalue,0,RX_BUFFER_SIZE);

    SERIAL.println("HT PROTOCOL VERSION 0.0");
    digitalWrite(LED_BUILTIN, HIGH);

    while(1){

        char error_flag = NO_RESPONSE;
        memset(buffer, 0, RX_BUFFER_SIZE);
        int msg_len = SERIAL.readBytesUntil('\n', buffer, RX_BUFFER_SIZE);

        if(msg_len){

          // PROTOCOL
            if(strstr(buffer, "GET TARGET_MIN_TEMP")){
                SERIAL.println(configuration.target_min_temp);
                error_flag = OK_RESPONSE;
            }

            if(strstr(buffer, "SET TARGET_MIN_TEMP")){
                sscanf(buffer, "SET TARGET_MIN_TEMP %s", svalue);

                if(isDouble(svalue)){
                    configuration.target_min_temp = strtod(svalue, NULL);
                    EEPROM_writeAnything(0, configuration);
                    error_flag = OK_RESPONSE;
                }else{
                    error_flag = ERROR_RESPONSE;
                }
          }

            // No valid command found
            //if(error_flag != OK_RESPONSE && error_flag != ERROR_RESPONSE){
            if(error_flag == NO_RESPONSE){
                error_flag = NOT_FOUND_RESPONSE;
            }

           // RESPONSE
            switch(error_flag){

                case OK_RESPONSE:
                    SERIAL.println("OK");
                    break;

                case NOT_FOUND_RESPONSE:
                    SERIAL.print("ERROR ");
                    SERIAL.println(NOT_FOUND_RESPONSE);
                    break;

                case ERROR_RESPONSE:
                    SERIAL.print("ERROR ");
                    SERIAL.println(ERROR_RESPONSE);
                    break;

                default:
                    break;
            } // end switch

        } // end if msg_len
    } // end while
} // end
