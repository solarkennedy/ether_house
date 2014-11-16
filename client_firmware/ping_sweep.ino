
void ping_sweep() {
  // Assumes class C. 
  // TODO: Be smarter about the range
  syslog("Starting Ping Sweep");
  for (int i = 1; i < 255; i++) {
    uint8_t ip[4];
    ip[0] = ether.myip[0]; 
    ip[1] = ether.myip[1]; 
    ip[2] = ether.myip[2];
    ip[3] = i; 
    // Don't care about replies, just want a ping sweep.
    ether.clientIcmpRequest(ip);
  }
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

