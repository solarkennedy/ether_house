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
   char* a = root["1"]["mac"];
   Serial.println(a);
 
   Serial.println("Decoding all json");
   for (JsonObjectIterator i=root.begin(); i!=root.end(); ++i) {
    Serial.println(i.key());
    Serial.println((char*)i.value());
   }   
  }
  
  Serial.println("Raw Output:");
  Serial.print((const char*) Ethernet::buffer + off +  seek_location);
  Serial.println();
  Serial.println("-------------");
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
 
