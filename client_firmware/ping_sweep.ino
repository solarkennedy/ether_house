void ping_sweep() {
  // Assumes class C. 
  // TODO: Be smarter about the range
  uint8_t ip[4];
  ip[0] = ether.myip[0];
  ip[1] = ether.myip[1];
  ip[2] = ether.myip[2];
  syslog("Starting Ping Sweep");
  Serial.print("\nPinging the ");
  Serial.print(ip[0]);
  Serial.print(".");
  Serial.print(ip[1]);
  Serial.print(".");
  Serial.print(ip[2]);
  Serial.print(".0/24 subnet...");
  for (int i = 1; i < 255; i++) {
    ip[3] = i; 
    // Don't care about replies, just want a ping sweep.
    ether.clientIcmpRequest(ip);
    // But we must use our customPacketLoop in order to properly intercept
    // Sniffed packets as they come back!
    delay(2);
    ether.customPacketLoop(ether.packetReceive());
    delay(2);
  }
  Serial.println("done.");
}
void ping_target() {
  // Don't bother trying to ping anything that isn't local to the lan.
  if (is_ip_local(target_ip) == true) {
    //ether.printIp("Pinging target ip: ", target_ip);
    ether.clientIcmpRequest(target_ip);
  }
}

// Filter to only look at packets that are coming from a legit ip.
boolean is_ip_local(byte ip[4]) {
  if (memcmp(ip, allOnes, 4) == 0 || memcmp(ip, allZeros, 4) == 0 ) {
    return false;
  }
  for(int i = 0; i < 4; i++)
    if((ether.myip[i] & EtherCard::netmask[i]) != (ip[i] & EtherCard::netmask[i])) {
      return false;
    }
  // If we got here then it is legit
  return true;
}

// validate_dhcp checks to make sure that our dhcp lease is still valid
// and initiates a reboot if not. This sometimes happens :(
void validate_dhcp() {
  if (is_ip_local(ether.myip) == false) {
    syslog("Invalid dhcp");
    reboot();
  }
}
