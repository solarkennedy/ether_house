
void printMac(uint8_t macaddr[6]) {
  Serial.print(macaddr[0], HEX);
  int i;
  char tmp[16];
  for (i=1; i<6; i++) {
    sprintf(tmp, "%.2X",macaddr[i]); 
    Serial.print(':'); 
    Serial.print(tmp);
  }
  Serial.println();
}

void printState(byte the_state) {
  Serial.println("Current state:");
  for (int i=0; i<NUM_HOUSES; i++) {
    boolean onoroff = (state >> i) & 1;
    Serial.print("House "); 
    Serial.print(i); 
    Serial.print(": "); 
    Serial.println(onoroff); 
  }
}

void print_netcfg() {
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
  ether.printIp("Server: ", ether.hisip);
  Serial.println();
}

