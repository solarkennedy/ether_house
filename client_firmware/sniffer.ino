//callback that prints received packets to the serial port
void packet_sniffer_callback(uint8_t srcmacaddr[6], byte ip[4], const char *data, word len) {
  Serial.print("Got Sniffed Packet from: ");
  ether.printIp(ip);
  Serial.println();
  // If we got a packet, we need to reset the absense_timer so it can start counting down again
  absense_timer = millis();
  turn_my_house_on();

  // Only copy the ip as a target if it is local.
  // Everything else is bogus.
  if (is_ip_local(ip) == true) {
    memcpy(target_ip, ip, sizeof target_ip);
  }
}



