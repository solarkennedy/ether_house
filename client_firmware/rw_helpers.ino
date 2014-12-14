// Read Write Helpers for eeprom management

void saveStateToEeprom() {
  cli();
  if(EEPROM.read(STATE_ADDR) != state) {
    EEPROM.write(STATE_ADDR, state);
  }
  sei();
}

void readStateFromEeprom() {
  cli();
  state = EEPROM.read(STATE_ADDR);
  sei();
  sync_leds();
}
