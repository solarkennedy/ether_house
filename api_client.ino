void set_target_mac() {
  Serial.println("Retrieving target MAC address from server...");
  ether.browseUrl(PSTR("/macs.json?id="), my_id_char , api_server, macs_parse_callback);
  while (target_mac[0] == 255) { 
    ether.packetLoop(ether.packetReceive());
  }
  delay(10);
}


static void macs_parse_callback (byte status, word off, word len) {
  Serial.println("Entering macs_parse_callback");
  JsonParser<10> parser2;
  int seek_location = find_response( Ethernet::buffer + off, len);
  //TODO Error handling
  JsonArray root2 = parser2.parse((char*)(Ethernet::buffer + off + seek_location));
  uint8_t received_mac[6] = { 
    0,0,0,0,0,0   };
  for ( int i=0 ; i<6 ; i++ ) {
    received_mac[i] = floor(root2[i]);
  }
  // Now that we have a MAC to look for, save it to target_mac
  memcpy(target_mac, received_mac, sizeof received_mac); 
  // Configure a callback for our target mac:
  Serial.print("Enabling listener for MAC: ");
  printMac(target_mac);
  ether.snifferListenForMac(&PrintPacket, target_mac);
}


void set_initial_state() {
  Serial.println();
  Serial.println("Entering set_initial_state");
  Serial.print("State is currently:"); Serial.println(state);
  ether.browseUrl(PSTR("/state.json"), "", api_server, state_parse_callback);
  Serial.println("Set callback for state.json");
  while (state == 255) {
    ether.packetLoop(ether.packetReceive());
  }
  delay(10);
  Serial.println("Leaving set_initial_state");
}


void state_parse_callback (byte status, word off, word len) {
  Serial.println("Entering state_parse_callback");
  JsonParser<12> parser;
  int seek_location = find_response( Ethernet::buffer + off, len);
  JsonArray root = parser.parse((char*)(Ethernet::buffer + off + seek_location));
  // TODO Error handling
  for ( int i=0 ; i<num_houses ; i++ ) {
    // Use bitwise math to store the exact i bit into the state byte
    int x = floor(root[i]);
    bitWrite(state, i, x);
  }
  Serial.print("State in decimal: "); 
  Serial.println(state);
  printState(state);
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


