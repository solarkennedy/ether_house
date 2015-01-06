//callback that prints received packets to the serial port
void packet_sniffer_callback(const uint8_t *src_mac, const uint8_t *src_ip) {
  static bool sniffer_nested;

  if (sniffer_nested)
    return;

  sniffer_nested = true;

  // If we got a packet, we need to reset the absense_timer so it can start counting down again
  // even if the packet isn't IP
  absense_timer = millis();
  turn_my_house_on();
  Serial.print(".");

  // Only copy the ip as a target if it is local.
  // Everything else is bogus.
  if (is_ip_local(src_ip)) {
    // Only print an store if the sniffed ip is new
    if (memcmp(src_ip, target_ip, 4) != 0){
      Serial.print(F("\nGot sniffed local ip packet. Storing target: "));
      ether.printIp(src_ip);
      Serial.println();
      memcpy(target_ip, src_ip, 4);
    }
  }

  sniffer_nested = false;
}
