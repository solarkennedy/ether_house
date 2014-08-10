// Interactions with the remote API

const char api_server[] PROGMEM = "archive";


// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}


void get_initial_config() {
  
  Serial.println("Fetching intial config from the API");
  
  if (!ether.dnsLookup(api_server)) {
    Serial.println("DNS failed");
    // TODO: Sleep then reset
  }
  ether.printIp("Server: ", ether.hisip);
  
  ether.browseUrl(PSTR("/config"), "", api_server, my_callback);
  
  // TODO Make this come from the API
  //static byte got_mac[] = { 0x60,0xbe,0xb5,0x8f,0x3d,0xa7 };
  static byte got_mac[6] = { 0xc4, 0x85, 0x08, 0x31, 0x7e, 0x73 };

  memcpy(target_mac, got_mac, sizeof got_mac);  
  
}
