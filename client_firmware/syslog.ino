// Sends syslog packet to the broadcast address and prints to stdout
// Probably not RFC5424 compliant...

static inline void _syslog(char *buf, int string_size) {
  ether.sendUdp(buf, string_size - 1, 514, allOnes, 514);
}

// News + INFO?
static const char prefix[] PROGMEM = "<62>etherhouse";

void syslog(const __FlashStringHelper *logstring) {
  int string_size = (sizeof(prefix) - 1) + 1 + 1 + strlen_P((const char *)logstring) + 1;
  char buf[string_size];
  char *s;

  Serial.println(logstring);

  strcpy_P(buf, prefix);
  s = buf + sizeof(prefix) - 1;
  *s++ = '0' + MY_ID;
  *s++ = ' ';
  strcpy_P(s, (const char *)logstring);

  _syslog(buf, string_size);
}

void syslog(const char *logstring) {
  int string_size = (sizeof(prefix) - 1) + 1 + 1 + strlen_P((const char *)logstring) + 1;
  char buf[string_size];
  char *s;

  Serial.println(logstring);

  strcpy_P(buf, prefix);
  s = buf + sizeof(prefix) - 1;
  *s++ = '0' + MY_ID;
  *s++ = ' ';
  strcpy_P(s, (const char *)logstring);

  _syslog(buf, string_size);
}
