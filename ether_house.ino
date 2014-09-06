#include <EtherCard.h>
#include <IPAddress.h>
#define CSPIN 10
#define REQUEST_RATE 50000
// Ping our target every 2 seconds
#define PINGER_RATE 2000
// Ping everything every hour
#define PINGSWEEP_RATE 3600000
#define TIMEOUT 10000
// A device is gone if we haven't heard from them in 15 minutes
#define ABSENSE_TIMEOUT 90000
//#define ABSENSE_TIMEOUT 900000

#define NUM_HOUSES 8
#define MY_ID 0
#define MY_ID_CHAR "0"

const byte my_mac[] = {   0x74,0x69,0x69,0x2D,0x30,0x31 };
const byte allZeros[] = { 0x00, 0x00, 0x00, 0x00 };
const byte allOnes[] = { 0xFF, 0xFF, 0xFF, 0xFF };
const char api_server[] PROGMEM = "archive.gateway.2wire.net";

uint8_t target_mac[6] = {   -1,-1,-1,-1,-1,-1 };
byte target_ip[4] = { 255, 255, 255, 255 };
byte state = -1;
byte Ethernet::buffer[700];
static long timer;
static long pinger_timer;
static long absense_timer;
static long pingsweep_timer;

void setup () {
  Serial.begin(115200);
  Serial.println("\nether_house starting");
  setup_pins();

  if (ether.begin(sizeof Ethernet::buffer, my_mac, CSPIN) == 0) {
    Serial.println( "Failed to access Ethernet controller");
    reboot_after_delay();
  }
  if (!ether.dhcpSetup()) {
    Serial.println("DHCP failed");
    reboot_after_delay();
  }
  if (!ether.dnsLookup(api_server)) {
    Serial.println("DNS failed");
    reboot_after_delay();
  }

  print_netcfg();

  get_target_mac();
  get_remote_state(); 

  Serial.println("Finished initial configuration");
  Serial.println("Now entering main loop");
  
  // Setup timers
  pinger_timer = millis() - PINGER_RATE ; 
  // Start the absense timer with the total grace period to give it the benifit of the doubt
  absense_timer = millis();
  // We can start the ping sweep on bootup.
  pingsweep_timer = millis() - PINGSWEEP_RATE;
  
}

void loop () {
  // Normal loop of getting packets if they are available
  ether.customPacketLoop(ether.packetReceive());
  
//  // Ping our target to see if they are alive
//  if (millis() > pinger_timer + PINGER_RATE) {
//    pinger_timer = millis();
//    ping_target();
//  }
//  
//  // If we haven't heard from our device, time to time out and turn off
//  if ((millis() > absense_timer + ABSENSE_TIMEOUT) && (bitRead(state, MY_ID) == true)) {
//    Serial.println("Haven't heard from our target. Assuming it is gone.");
//    Serial.print("Millis: "); Serial.println(millis());
//    turn_off(MY_ID);
//  }
//  
//  // After a long time we ping everything in case we don't even know what ip our device has
//  if (millis() > pingsweep_timer + PINGSWEEP_RATE) {
//    pingsweep_timer = millis();
//    ping_sweep();
//  }
  
  // Cover the case where I am unplugged and must reset myself
//  if (ENC28J60::isLinkUp() == false)
//    reboot_after_delay();
  
}

void reboot() {
  Serial.println("Rebooting now.");
  delay(100);
  asm volatile ("  jmp 0");  
}

void reboot_after_delay() {
  Serial.println("Starting delay for a reboot.");
  delay(100000);
  reboot();
}
