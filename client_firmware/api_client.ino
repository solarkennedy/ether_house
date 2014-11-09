void get_target_mac() {
  Serial.println("Retrieving target MAC address from server...");
  ether.browseUrl(PSTR("/target_mac?id=" MY_ID_CHAR "&api_key=" MY_API_KEY), "" , api_server, macs_parse_callback);
  uint32_t timer = millis() + HTTP_TIMEOUT;
  while (target_mac[0] == 255) { 
    if (millis() > timer) {
      Serial.println("Timeout occured.");
      reboot(); 
    }
    ether.packetLoop(ether.packetReceive());
  }
  while( ether.packetLoop(ether.packetReceive()));
}

static void macs_parse_callback (byte status, word off, word len) {
  Serial.println("Entering macs_parse_callback");
  int seek_location = find_response(Ethernet::buffer + off, len);
  uint8_t received_mac[6] = { 
    0,0,0,0,0,0       };
  // Now that we have a MAC to look for, save it to target_mac
  memcpy(target_mac, (Ethernet::buffer + off + seek_location), sizeof received_mac); 
  // Configure a callback for our target mac:
  Serial.print("Enabling listener for MAC: ");
  printMac(target_mac);
  // While we still a connection, let it it wait for the syn/ack stuff
  ether.snifferListenForMac(&packet_sniffer_callback, target_mac);
}

void get_remote_state() {
  Serial.println();
  Serial.println("Entering set_initial_state");
  Serial.print("State is currently:"); 
  Serial.println(state);
  ether.browseUrl(PSTR("/state?id=" MY_ID_CHAR "&api_key=" MY_API_KEY), "", api_server, state_parse_callback);
  uint32_t timer = millis() + HTTP_TIMEOUT;
  while (state == 255) {
    ether.packetLoop(ether.packetReceive());
    if (millis() > timer) {
      Serial.println("Timeout occured.");
      reboot_after_delay(); 
    }
  }
  // While we still a connection, let it it wait for the syn/ack stuff
  while( ether.packetLoop(ether.packetReceive()));
  Serial.println("Leaving set_initial_state");
}

void state_parse_callback (byte status, word off, word len) {
  Serial.println("Entering state_parse_callback");
  int seek_location = find_response( Ethernet::buffer + off, len);
  memcpy(&state, (Ethernet::buffer + off + seek_location), sizeof state);
  state = (state>>4) | (state<<4);
  Serial.print("State in decimal: "); 
  Serial.println(state);
  printState(state);
  sync_leds();
}

void api_set_off() {
  ether.browseUrl(PSTR("off?id=" MY_ID_CHAR "&api_key=" MY_API_KEY), "", api_server, api_set_callback);
}

void api_set_on() {
  ether.browseUrl(PSTR("on?id=" MY_ID_CHAR "&api_key=" MY_API_KEY), "", api_server, api_set_callback);
}

void api_set_callback (byte status, word off, word len) {
  Serial.println("Entering state_parse_callback");
  int seek_location = find_response( Ethernet::buffer + off, len);
  // TODO Error handling
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
      return foundpos + needle_length;
    }
  }
  return foundpos;
}

void sync_my_state() {
  if (bitRead(state, MY_ID) == 1)
    api_set_on();
  else
    api_set_off();
  get_remote_state();
}

