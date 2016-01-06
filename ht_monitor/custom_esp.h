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

    const char json_template[] = "{\"get_humidity\":%d, \"get_temperature\":%d}\n";
    memset(json_payload, 0, len_json_payload);
    sprintf(json_payload, json_template, get_humidity, get_temperature);

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

    // HTTP headers - 88 bytes + strlen(%d) + strlen(%s)
    const char http_headers[] =
    "POST / HTTP/1.0\r\n"
    "Content-Type:application/json \r\n"
    "Connection: close\r\n"
    "Content-Length: %d\r\n"
    "\r\n"
    "%s";

    memset(tcp_payload, 0, len_tcp_payload);
    sprintf(tcp_payload, http_headers, len_http_payload, http_payload);

    return true;
}
