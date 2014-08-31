void turn_on(int house) {
  toggle_house(house, true);
}

void turn_off(int house){
  toggle_house(house, false);
}

void toggle_house(int house, boolean value) {
  // If the state is actually different than what we currently have
  if (bitRead(state, house) != value) {
    Serial.print("Turning house "); 
    Serial.print(house);
    Serial.print(" to value "); 
    Serial.println(value);
    bitWrite(state, house, value);
    sync_leds();
  }
}

void sync_leds() {
  for ( int i=0 ; i<NUM_HOUSES ; i++ ) {
    digitalWrite(i+2, bitRead(state, i));
  }
}

void setup_pins() {
  for ( int i=0 ; i<NUM_HOUSES ; i++ ) {
    pinMode(i+2, OUTPUT);
  }
}



