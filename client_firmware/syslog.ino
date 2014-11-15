// Sends syslog packet to the broadcast address and prints to stdout
// Probably not RFC5424 compliant...
void syslog(const char* logstring) {
  Serial.println(logstring);
  String packet;
  // News + INFO?
  packet = "<62>etherhouse"MY_ID_CHAR " " ;
  packet += logstring;
  packet += " ";

  int string_size=packet.length();
  char buf[string_size+1];
  packet.toCharArray(buf,string_size);
  ether.sendUdp (buf, string_size, 514, allOnes, 514);

}
