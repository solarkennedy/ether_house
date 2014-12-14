void get_target_mac() {
  if (locked == true) {
    Serial.println(F("Get_target_mac aborted: locked by something else"));
    return;
  }
  syslog("Retrieving target MAC address from server...");
  ether.browseUrl(PSTR("/target_mac?id=" MY_ID_CHAR "&api_key=" MY_API_KEY), "" , api_server, macs_parse_callback);
  uint32_t timer = millis() + HTTP_TIMEOUT;
  locked = true;
  while (locked == true) { 
    ether.packetLoop(ether.packetReceive());
    if (millis() > timer) {
      syslog("Timeout occured when trying to get mac");
      reboot(); 
    }
  }
  wait_for_tcp();
}

static void macs_parse_callback (byte status, word off, word len) {
  int seek_location = find_response(Ethernet::buffer + off, len);
  if (seek_location == -1) {
    Serial.println(F("Got a non-ok response from target_mac"));
  }   
  else {
    memcpy(target_mac, (Ethernet::buffer + off + seek_location), sizeof target_mac); 
    // Configure a callback for our target mac:
    Serial.print(F("Enabling listener for MAC: "));
    printMac(target_mac);
    // While we still a connection, let it it wait for the syn/ack stuff
    ether.snifferListenForMac(&packet_sniffer_callback, target_mac);
  }
  locked = false;
}

void get_remote_state() {
  if (locked == true) {
    Serial.println(F("Remote state fetch aborted: locked by something else"));
    return;
  }
  syslog("Syncing State from Server.");
  ether.browseUrl(PSTR("/state?id=" MY_ID_CHAR "&api_key=" MY_API_KEY), "", api_server, state_parse_callback);
  uint32_t timer = millis() + HTTP_TIMEOUT;
  locked == true;
  while (locked == true) {
    ether.packetLoop(ether.packetReceive());
    if (millis() > timer) {
      syslog("Timeout occured when trying to fetch state");
      reboot_after_delay(); 
    }
  }
  wait_for_tcp();
}

void state_parse_callback (byte status, word off, word len) {
  int seek_location = find_response( Ethernet::buffer + off, len);
  if (seek_location == -1) {
    Serial.println(F("Got a non-ok response from state_parse"));
  }   
  else {
    memcpy(&state, (Ethernet::buffer + off + seek_location), sizeof state);
    char buf[25];
    sprintf(buf, "Synced State is "BYTETOBINARYPATTERN, BYTETOBINARY(state));
    syslog(buf);
    sync_leds();
    saveStateToEeprom();
  }
  locked = false;
}

void api_set_off() {
  if (locked == true) {
    Serial.println(F("API set off aborted: locked by something else"));
    return;
  }
  syslog("Sending OFF for my house: "MY_ID_CHAR);

  ether.browseUrl(PSTR("/off?id=" MY_ID_CHAR "&api_key=" MY_API_KEY), "", api_server, api_set_callback);
  uint32_t timer = millis() + HTTP_TIMEOUT;
  locked = true;
  while (locked == true) {
    ether.packetLoop(ether.packetReceive());
    if (millis() > timer) {
      syslog("Timeout occured when trying to set on/off");
      reboot_after_delay(); 
    }
  }
  wait_for_tcp();
}

void api_set_on() {
  if (locked == true) {
    Serial.println(F("API set on aborted: locked by something else"));
    return;
  }
  syslog("Sending ON for my house: "MY_ID_CHAR);
  ether.browseUrl(PSTR("/on?id=" MY_ID_CHAR "&api_key=" MY_API_KEY), "", api_server, api_set_callback);
  uint32_t timer = millis() + HTTP_TIMEOUT;
  locked = true;
  while (locked == true) {
    ether.packetLoop(ether.packetReceive());
    if (millis() > timer) {
      syslog("Timeout occured when trying to set on/off");
      reboot_after_delay(); 
    }
  }
  wait_for_tcp();
}

void api_set_callback (byte status, word off, word len) {
  Serial.println("Entering api_set_callback");
  int seek_location = find_response( Ethernet::buffer + off, len);
  if (seek_location == -1) {
    Serial.println(F("bad response from set_api_callback"));
  }   
  locked = false;
  Serial.println(F("Finished set api call OK."));
}

// find_response
// Returns a integer offset where a http response starts.
// Returns -1 if it couldn't find the delimiter between the headers and response
int find_response( byte* haystack, int length) {
  char needle[] = "\r\n\r\n";
  int foundpos = -1;
  int needle_length = sizeof needle - 1;
  for (int i = 0; (i < length - needle_length); i++) {
    if (memcmp(needle, haystack + i, needle_length) == 0) {
      foundpos = i;
      if (is_200(haystack, length)== true) {
        return foundpos + needle_length;
      } 
      else {
        return -1;
      }
    }
  }
  return foundpos;
}

bool is_200(byte* haystack, int length) {
  char needle[] = "200 OK";
  int needle_length = sizeof needle - 1;
  for (int i = 0; (i < length - needle_length); i++) {
    if (memcmp(needle, haystack + i, needle_length) == 0) {
      return true;
    }
  }
  Serial.println(F("No 200 Found"));
  return false;
}

void wait_for_tcp() {
  uint32_t timer = millis() + HTTP_TIMEOUT;
  // While we still a connection, let it it wait for the syn/ack stuff
  while(millis() < timer){
    ether.packetLoop(ether.packetReceive());
  }
}





