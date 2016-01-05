/*
String connect(){
  wifi.setOprToStation(); // Set operation mode to staion.
  wifi.joinAP (configuration.ssid, configuration.passwd); //Join in AP.
  return wifi.getLocalIP(); // Get the IP address of ESP8266.
  //leaveAP(); Leave AP joined before.
}
*/

bool dht_json_wrapper(char *json_payload,
                                  const uint8_t len_json_payload,
                                  const uint8_t get_humidity,
                                  const uint8_t get_temperature){

    // Cleans tcp_payload
    memset(json_payload, 0, len_json_payload);
    // Formats json
    sprintf(json_payload, "{\"get_humidity\":%d, \"get_temperature\":%d}", get_humidity, get_temperature);
           
    return true;                
}

bool http_wrapper(char *tcp_payload,
                                  const uint8_t len_tcp_payload,
                                  const char *http_payload,
                                  const uint8_t len_http_payload){
 
    /*
    
    tcp_payload -> It the return of http_wrapper
                           -> this cstring might be sent with the ESP8266 module.
                           -> make sure its big enough !
                           -> the headers and the payload will be written here
                           -> tcp_payload content will be overwritten !

    len_tcp_payload -> number of allocated bytes for tcp_payload
    http_payload -> The content of your POST request 
    len_http_payload -> strlen of your POST request
    */
                                    
    // HTTP headers
    char http_headers[] = 
    "POST / HTTP/1.0\r\n"
    "Content-Type:application/json \r\n"
    "Connection: close\r\n";
 
    // Converts int to char
    char http_payload_length[8] = "0";
    sprintf(http_payload_length, "%d", len_http_payload);
 
    // Cleans tcp_payload
    memset(tcp_payload, 0, len_tcp_payload);
    
    strcat(tcp_payload, http_headers);
    strcat(tcp_payload, "Content-Length: ");
    strcat(tcp_payload, http_payload_length);
    strcat(http_headers, "\r\n\r\n");
    strcat(http_headers, http_payload);
    
    return true;
}
