void turn_on(int house) {
  bitWrite(state, house, 1);
  sync_leds();
}

void turn_off(int house){
  bitWrite(state, house, 0);
  sync_leds();
}

void sync_leds() {
  for ( int i=0 ; i<num_houses ; i++ ) {
    digitalWrite(i+2, bitRead(state, i));
  }
}

void setup_pins() {
  for ( int i=0 ; i<num_houses ; i++ ) {
    pinMode(i+2, OUTPUT);
  }
}

