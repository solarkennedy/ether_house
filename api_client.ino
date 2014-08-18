// Interactions with the remote API
const char api_server[] PROGMEM = "archive";


// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  
  JsonParser<32> parser;
  
  int seek_location = find_response( Ethernet::buffer + off, len);
  Serial.print("Found location in "); Serial.println(seek_location);
  Serial.print("The offset was "); Serial.println(off);
  
  JsonObject root = parser.parse((char*)(Ethernet::buffer +off + seek_location));
  if (!root.success()) {
    Serial.println("Failed to decode JSON");
  } else {
 

  }
  
  Serial.println("Raw Output:");
  Serial.print((const char*) Ethernet::buffer + off +  seek_location);
  Serial.println();
  Serial.println("-------------");
}


void set_target_mac() {
  ether.browseUrl(PSTR("/macs.json"), "", api_server, macs_parse_callback);
}
static void macs_parse_callback (byte status, word off, word len) {
  JsonParser<32> parser2;
  int seek_location = find_response( Ethernet::buffer + off, len);
  JsonArray root2 = parser2.parse((char*)(Ethernet::buffer + off + seek_location));
  uint8_t received_mac[6] = { 0,0,0,0,0,0 };
  for ( int i=0 ; i<6 ; i++ ) {
    received_mac[i] = floor(root2[0][i]);
  }
  Serial.print("The MAC to look for appears to be: ");
  printMac(received_mac);
  memcpy(target_mac, received_mac, sizeof received_mac); 
  
  ether.snifferListenForMac(&PrintPacket, target_mac);
  Serial.print("Enabling listener for MAC: ");
  printMac(target_mac);
}


void get_initial_config() {
  
  Serial.println("Fetching intial config from the API");
  
  if (!ether.dnsLookup(api_server)) {
    Serial.println("DNS failed");
    // TODO: Sleep then reset
  }
 
  ether.printIp("Server: ", ether.hisip);
 
  set_target_mac(); 
 // ether.browseUrl(PSTR("/config"), "", api_server, my_callback);
  
  
}

// find_response
// Returns a integer offset where a http response starts.
// Returns -1 if it couldn't find the delimiter between the headers and response
int find_response( byte* haystack, int length) {
  char needle[] = "\r\n\r\n";
 // Serial.print("Searching for my needle: "); Serial.println(needle);
  int foundpos = -1;
  int needle_length = sizeof needle - 1;
//  Serial.print("Needle length: "); Serial.println(needle_length);
  for (int i = 0; (i < length - needle_length); i++) {
  //  Serial.print("searching at ");     Serial.print(i);
  //  Serial.print("   Value: "); Serial.write(haystack[i]); Serial.println();
    if (memcmp(needle, haystack + i, needle_length) == 0) {
      foundpos = i;
   //   Serial.println("Found!");
      return foundpos + needle_length;
    }
  }
  return foundpos;
}
 
