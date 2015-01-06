void ping_sweep() {
  // Assumes class C.
  // TODO: Be smarter about the range
  uint8_t ip[4];
  ip[0] = ether.myip[0];
  ip[1] = ether.myip[1];
  ip[2] = ether.myip[2];
  syslog(F("Starting Ping Sweep"));
  Serial.print("Pinging the ");
  Serial.print(ip[0]);
  Serial.print(".");
  Serial.print(ip[1]);
  Serial.print(".");
  Serial.print(ip[2]);
  Serial.print(".0/24 subnet...");
  for (int i = 1; i < 255; i++) {
    ip[3] = i;
    if (!memcmp(ip, EtherCard::myip, 4))
        continue;
    // Don't care about replies, just want a ping sweep.
    // If ARP succeeded, we processed the ARP response in the sniffer,
    // so we saw/tested the MAC, so there's no need to ICMP ping.
    // If ARP didn't succeed, we can't ICMP ping, since we don't know the MAC
    ether.selectAndArpDestAddr(ip, 5);
    delay(2);
  }
  Serial.println("done.");
}
void ping_target() {
  // Don't bother trying to ping anything that isn't local to the lan.
  if (is_ip_local(target_ip)) {
    //ether.printIp("Pinging target ip: ", target_ip);
    // We must ICMP ping here, since some (at least Android) systems don't
    // respond to ARP when asleep, but an ICMP pings wakes them up to respond.
    // This is probably due to buggy TCP offload processors. See e.g.
    // https://code.google.com/p/android/issues/detail?id=42272
    //
    // Of course, if we've communicated with some other destination in the
    // mean time, the stored destination MAC address has probably been
    // over-written with the API server or ping sweep IP, so in theory we
    // must ARP for it again.
    //
    // However, since ARP isn't going to work per the above, we just force
    // the destination MAC back to the target_mac.
    // simply over-write the destination MAC rather than ARPing for it...
    ether.forceDestMac(target_mac);
    ether.clientIcmpRequest(target_ip);
  }
}

// Filter to only look at packets that are coming from a legit ip.
boolean is_ip_local(const uint8_t *ip) {
  if (memcmp(ip, allOnes, 4) == 0 || memcmp_P(ip, allZeros, 4) == 0 ) {
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
  if (!is_ip_local(ether.myip)) {
    syslog(F("Invalid dhcp"));
    reboot();
  }
}
