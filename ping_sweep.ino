// TODO Periodic ping sweeps to wake up the thing we are looking out for

void ping_sweep() {
  // Assumes class C. 
  // TODO: Be smarter about the range
  Serial.println("Beginning ping sweep. Starting at 1 and going to 254.");
  for (int i = 1; i < 255; i++) {
    Serial.print(".");
    uint8_t ip[4];
    ip[0] = ether.myip[0]; ip[1] = ether.myip[1]; ip[2] = ether.myip[2];
    ip[3] = i; 
    // Don't care about replies, just want a ping sweep.
    ether.clientIcmpRequest(ip);
  }
  Serial.println();
  Serial.println("Ping sweep finished");
  
}
