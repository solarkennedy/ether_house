void turn_on(int house) {
  bitWrite(state, house, 1);
  sync_leds();
}

void turn_off(int house){
  bitWrite(state, house, 0);
  sync_leds();
}

void sync_leds() {
  boolean led=0;
  for ( int i=0 ; i<num_houses ; i++ ) {
    led = bitRead(state, i);
    Serial.print("Setting led "); Serial.print(i); Serial.print(" to value "); Serial.println(led);
    digitalWrite(i+2, led);
  }
}

void setup_pins() {
  for ( int i=0 ; i<num_houses ; i++ ) {
    pinMode(i+2, OUTPUT);
  }
}

