void printMac(const uint8_t *macaddr) {
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

void printState(uint8_t the_state) {
  Serial.println("Current state:");
  for (int i=NUM_HOUSES-1; i>=0; i--) {
    boolean onoroff = (state >> i) & 1;
    Serial.print(F("House "));
    Serial.print(i);
    Serial.print(F(": "));
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

