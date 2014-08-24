

//callback that prints received packets to the serial port
void PrintPacket(uint8_t srcmacaddr[6], byte ip[4], const char *data, word len) {
  IPAddress src(ip[0], ip[1], ip[2], ip[3]);
  Serial.println("Got Sniffed Packet!");
  Serial.print("Src ip:  "); 
  Serial.println(src);
  Serial.print("Src Mac: "); 
  printMac(srcmacaddr);
  Serial.print("Data:    "); 
  Serial.println(data);
  Serial.print("Size:    "); 
  Serial.println(len);
  Serial.println();
}

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
  for (int i=0; i<num_houses; i++) {
    boolean onoroff = (state >> i) & 1;
    Serial.print("House "); 
    Serial.print(i); 
    Serial.print(": "); 
    Serial.println(onoroff); 
  }
}

