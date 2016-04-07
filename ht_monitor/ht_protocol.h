#include <avr/interrupt.h>
//#include "AltSoftSerial.h"
#include "is_double.h"

#define SSID_SIZE 32
#define PASSWD_SIZE 32
#define API_ENDPOINT_SIZE 32
#define IPADDR_SIZE 16
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

//#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL cmdSerial
SoftwareSerial DEBUG_SERIAL(9, 8); /* RX:D10, TX:D11 */
//AltSoftSerial DEBUG_SERIAL; /* RX:8  TX:9 Unusable:10 */

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
    char api_endpoint[API_ENDPOINT_SIZE];
    char api_ip_addr[IPADDR_SIZE];
} configuration;

void htProtocolSetup();
void htProtocolTrigger();
void htProtocolLoop();

void htProtocolSetup(){
  // DEBUG_SERIAL.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  EEPROM_readAnything(0, configuration);

  attachInterrupt(ISR_ADDR, htProtocolTrigger, RISING);

}

void htProtocolTrigger(){
   detachInterrupt(ISR_ADDR);
   ht_state = HIGH;
   digitalWrite(LED_BUILTIN, HIGH);
}

void htProtocolLoop(){

    char buffer[RX_BUFFER_SIZE] = "";
    //memset(buffer,0,RX_BUFFER_SIZE);
    DEBUG_SERIAL.setTimeout(RX_TIMEOUT_MS);

    float fvalue;
    char svalue[RX_BUFFER_SIZE] = "";
    memset(svalue,0,RX_BUFFER_SIZE);

    DEBUG_SERIAL.println("\nHT PROTOCOL VERSION 0.0\n");
    digitalWrite(LED_BUILTIN, HIGH);

    #ifdef E2PROM_DEBUG
      DEBUG_SERIAL.print("TARGET_MIN_TEMP: ");
      DEBUG_SERIAL.println(configuration.target_min_temp);
      DEBUG_SERIAL.print("TARGET_MAX_TEMP: ");
      DEBUG_SERIAL.println(configuration.target_temp_max);
      DEBUG_SERIAL.print("MIN_TEMP: ");
      DEBUG_SERIAL.println(configuration.min_temp);
      DEBUG_SERIAL.print("TEMP_MAX: ");
      DEBUG_SERIAL.println(configuration.temp_max);
      DEBUG_SERIAL.print("SSID: ");
      DEBUG_SERIAL.println(configuration.ssid);
      DEBUG_SERIAL.print("PASSWD: ");
      DEBUG_SERIAL.println(configuration.passwd);
      DEBUG_SERIAL.print("API_IP_ADDR: ");
      DEBUG_SERIAL.println(configuration.api_ip_addr);
      DEBUG_SERIAL.print("API_ENDPOINT: ");
      DEBUG_SERIAL.println(configuration.api_endpoint);
    #endif

    while(1){
        DEBUG_SERIAL.listen();
        char error_flag = NO_RESPONSE;
        memset(buffer, 0, RX_BUFFER_SIZE);
        int msg_len = DEBUG_SERIAL.readBytesUntil('\n', buffer, RX_BUFFER_SIZE);

        if(msg_len){

            // PROTOCOL
            /* ********************** TARGET_MIN_TEMP ********************** */

            if(strstr(buffer, "GET TARGET_MIN_TEMP")){
                DEBUG_SERIAL.println(configuration.target_min_temp);
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
            /* ********************** SSID ********************** */

             if(strstr(buffer, "GET SSID")){
                DEBUG_SERIAL.println(configuration.ssid);
                error_flag = OK_RESPONSE;
            }

            if(strstr(buffer, "SET SSID")){
                sscanf(buffer, "SET SSID \"%[^\"]", svalue);

                if(svalue){
                    strcpy(configuration.ssid, svalue);
                    EEPROM_writeAnything(0, configuration);
                    error_flag = OK_RESPONSE;
                }else{
                    error_flag = ERROR_RESPONSE;
                }
            }
            /* ********************** PASSWD ********************** */

             if(strstr(buffer, "GET PASSWD")){
                DEBUG_SERIAL.println(configuration.passwd);
                error_flag = OK_RESPONSE;
             }

            if(strstr(buffer, "SET PASSWD")){
                sscanf(buffer, "SET PASSWD \"%[^\"]", svalue);

                if(svalue){
                    strcpy(configuration.passwd, svalue);
                    EEPROM_writeAnything(0, configuration);
                    error_flag = OK_RESPONSE;
                }else{
                    error_flag = ERROR_RESPONSE;
                }
            }
            /* ********************** API IP ADDR ********************** */

             if(strstr(buffer, "GET API_IP_ADDR")){
                DEBUG_SERIAL.println(configuration.api_ip_addr);
                error_flag = OK_RESPONSE;
             }

            if(strstr(buffer, "SET API_IP_ADDR")){
                sscanf(buffer, "SET API_IP_ADDR \"%[^\"]", svalue);

                if(svalue){
                    strcpy(configuration.api_ip_addr, svalue);
                    EEPROM_writeAnything(0, configuration);
                    error_flag = OK_RESPONSE;
                }else{
                    error_flag = ERROR_RESPONSE;
                }
            }
            /* ********************** API ENDPOINT ********************** */

             if(strstr(buffer, "GET API_ENDPOINT")){
                DEBUG_SERIAL.println(configuration.api_endpoint);
                error_flag = OK_RESPONSE;
             }

            if(strstr(buffer, "SET API_ENDPOINT")){
                sscanf(buffer, "SET API_ENDPOINT \"%[^\"]", svalue);

                if(svalue){
                    strcpy(configuration.api_endpoint, svalue);
                    EEPROM_writeAnything(0, configuration);
                    error_flag = OK_RESPONSE;
                }else{
                    error_flag = ERROR_RESPONSE;
                }
            }


            /* ********************** END ********************** */

            // No valid command found
            //if(error_flag != OK_RESPONSE && error_flag != ERROR_RESPONSE){
            if(error_flag == NO_RESPONSE){
                error_flag = NOT_FOUND_RESPONSE;
            }

           // RESPONSE
            switch(error_flag){

                case OK_RESPONSE:
                    DEBUG_SERIAL.println("OK");
                    break;

                case NOT_FOUND_RESPONSE:
                    DEBUG_SERIAL.print("ERROR ");
                    DEBUG_SERIAL.println(NOT_FOUND_RESPONSE);
                    break;

                case ERROR_RESPONSE:
                    DEBUG_SERIAL.print("ERROR ");
                    DEBUG_SERIAL.println(ERROR_RESPONSE);
                    break;

                default:
                    break;
            } // end switch

        } // end if msg_len
    } // end while
} // end
