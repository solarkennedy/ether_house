

//callback that prints received packets to the serial port
void PrintPacket(uint8_t srcmacaddr[6], byte ip[4], const char *data, word len) {
  IPAddress src(ip[0], ip[1], ip[2], ip[3]);
  Serial.println("Got Sniffed Packet!");
  Serial.print("Src ip:  "); Serial.println(src);
  Serial.print("Src Mac: "); printMac(srcmacaddr);
  Serial.print("Data:    "); Serial.println(data);
  Serial.print("Size:    "); Serial.println(len);
  Serial.println();
}

void udpSerialPrint(word port, byte ip[4], const char *data, word len) {
  IPAddress src(ip[0], ip[1], ip[2], ip[3]);
  Serial.println("Got UDP Packet!");
  Serial.println(src);
  Serial.println(port);
  Serial.println(data);
  Serial.println(len);
}

void printMac(uint8_t macaddr[6]) {
  Serial.print(macaddr[0], HEX);
  int i;
  char tmp[16];
  for (i=1; i<6; i++) {
    sprintf(tmp, "%.2X",macaddr[i]); 
    Serial.print(':'); Serial.print(tmp);
  }
  Serial.println();
}
