//callback that prints received packets to the serial port
void packet_sniffer_callback(uint8_t srcmacaddr[6], byte ip[4], const char *data, word len) {
  // If we got a packet, we need to reset the absense_timer so it can start counting down again
  // even if the packet isn't IP
  absense_timer = millis();
  turn_my_house_on();
  Serial.print(".");

  // Only copy the ip as a target if it is local.
  // Everything else is bogus.
  if (is_ip_local(ip) == true) {
    // Only print an store if the sniffed ip is new
    if (memcmp(ip, target_ip, 4) != 0){
      Serial.print(F("\nGot sniffed local ip packet. Storing target: "));
      ether.printIp(ip);
      Serial.println();
      memcpy(target_ip, ip, 4);
    }
  }
}
