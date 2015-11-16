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
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  EEPROM_readAnything(0, configuration);

  attachInterrupt(ISR_ADDR, htProtocolTrigger, RISING);
    
  #ifdef E2PROM_DEBUG 
    Serial.print("TARGET_MIN_TEMP: ");
    Serial.println(configuration.target_min_temp); 
    Serial.print("TARGET_MAX_TEMP: ");
    Serial.println(configuration.target_temp_max); 
    Serial.print("MIN_TEMP: ");
    Serial.println(configuration.min_temp); 
    Serial.print("TEMP_MAX: ");
    Serial.println(configuration.temp_max); 
    Serial.print("SSID: ");
    Serial.println(configuration.ssid); 
    Serial.print("PASSWD: ");
    Serial.println(configuration.passwd); 
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
    Serial.setTimeout(RX_TIMEOUT_MS);

    float fvalue;
    char svalue[RX_BUFFER_SIZE] = "";
    memset(svalue,0,RX_BUFFER_SIZE);

    Serial.println("HT PROTOCOL VERSION 0.0");
    digitalWrite(LED_BUILTIN, HIGH);

    while(1){

        char error_flag = NO_RESPONSE;
        memset(buffer, 0, RX_BUFFER_SIZE);
        int msg_len = Serial.readBytesUntil('\n', buffer, RX_BUFFER_SIZE);

        if(msg_len){

          // PROTOCOL
            if(strstr(buffer, "GET TARGET_MIN_TEMP")){
                Serial.println(configuration.target_min_temp);
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
                    Serial.println("OK");
                    break;

                case NOT_FOUND_RESPONSE:
                    Serial.print("ERROR ");
                    Serial.println(NOT_FOUND_RESPONSE);
                    break;

                case ERROR_RESPONSE:
                    Serial.print("ERROR ");
                    Serial.println(ERROR_RESPONSE);
                    break;

                default:
                    break;
            } // end switch

        } // end if msg_len
    } // end while
} // end
