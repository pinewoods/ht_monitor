#include <avr/interrupt.h>

#define SSID_SIZE 32
#define PASSWD_SIZE 32
#define E2PROM_DEBUG

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

void ht_protocol_setup();
void ht_protocol_trigger();
void ht_protocol_loop();


void ht_protocol_setup(){
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  EEPROM_readAnything(0, configuration);

  attachInterrupt(ISR_ADDR, ht_protocol_trigger, RISING);
    
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

void ht_protocol_trigger(){
   detachInterrupt(ISR_ADDR);
   ht_state = HIGH;
   digitalWrite(LED_BUILTIN, HIGH);
}

void ht_protocol_loop(){
  Serial.println("HT PROTOCOL VERSION 0.0");
  while(1){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

  }
}
  
 /* 

void get_target_min_temp(){
  // GET TARGET_MIN_TEMP
  Serial.println(configuration.target_min_temp);
}

void get_target_min_temp(String instr){
  // SET TARGET_MIN_TEMP {float}
  // TODO: Parse instr
 configuration.target_min_temp = instr.toFloat();
}
*/
