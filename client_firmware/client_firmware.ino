#include <EtherCard.h>
#include <IPAddress.h>
#define CSPIN 10
// NOTE: All of these timers are in Milliseconds!
// Ping our target every 2 seconds
#define PINGER_INTERVAL 2000
// Ping everything every hour
#define PINGSWEEP_INTERVAL 36000
// How often to check back to the server for sync updates
#define SYNC_INTERVAL 36000
// General timeout for API calls and such
#define HTTP_TIMEOUT 10000
// A device is gone if we haven't heard from them in 15 minutes
#define ABSENSE_TIMEOUT 30000
//#define ABSENSE_TIMEOUT 900000

#define NUM_HOUSES 8
#define MY_ID 0
#define MY_ID_CHAR "0"
#define MY_API_KEY "testkey"

const byte my_mac[] = { 
  0x74,0x69,0x69,0x2D,0x30,MY_ID };
const byte allZeros[] = { 
  0x00, 0x00, 0x00, 0x00 };
const byte allOnes[] = { 
  0xFF, 0xFF, 0xFF, 0xFF };
const char api_server[] PROGMEM = "etherhouse.xkyle.com";

uint8_t target_mac[6] = {   
  -1,-1,-1,-1,-1,-1 };
byte target_ip[4] = { 
  255, 255, 255, 255 };
byte state = -1;
byte Ethernet::buffer[500];
static long timer;
static long pinger_timer;
static long absense_timer;
static long pingsweep_timer;
static long sync_timer;

void setup () {
  Serial.begin(115200);
  Serial.println(F("\nether_house starting"));
  setup_pins();

  if (ether.begin(sizeof Ethernet::buffer, my_mac, CSPIN) == 0) {
    Serial.println(F("Failed to access Ethernet controller"));
    reboot_after_delay();
  }
  if (!ether.dhcpSetup()) {
    Serial.println(F("DHCP failed"));
    reboot_after_delay();
  }
  if (!ether.dnsLookup(api_server)) {
    Serial.println(F("DNS failed"));
    reboot_after_delay();
  }

  print_netcfg();

  syslog("Etherhouse booting!");

  get_target_mac();
  get_remote_state(); 

  Serial.println(F("Finished initial configuration"));
  Serial.println(F("Now entering main loop"));

  // Setup timers
  pinger_timer = millis() - PINGER_INTERVAL ; 
  // Start the absense timer with the total grace period to give it the benifit of the doubt
  absense_timer = millis();
  // We can start the ping sweep on bootup.
  pingsweep_timer = millis() - PINGSWEEP_INTERVAL;
  // We already got the state from above. Setup the next issue.
  sync_timer = millis() - SYNC_INTERVAL;

}

void loop () {
  // Normal loop of getting packets if they are available
  ether.customPacketLoop(ether.packetReceive());

  // Ping our target to see if they are alive
  if (millis() > pinger_timer + PINGER_INTERVAL) {
    pinger_timer = millis();
    ping_target();
  }

  // If we haven't heard from our device, time to time out and turn off
  if ((millis() > absense_timer + ABSENSE_TIMEOUT) && (bitRead(state, MY_ID) == true)) {
    syslog("Absense timeout of target. Turning off light "MY_ID_CHAR);
    turn_my_house_off();
  }

  // After a long time we ping everything in case we don't even know what ip our device has
  if (millis() > pingsweep_timer + PINGSWEEP_INTERVAL) {
    pingsweep_timer = millis();
    ping_sweep();
  }

  if (millis() > sync_timer + SYNC_INTERVAL) {
    sync_timer = millis();
    get_remote_state();
  }


}

void reboot() {
  syslog("Rebooting now.");
  delay(100);
  asm volatile ("  jmp 0");  
}

void reboot_after_delay() {
  delay(100000);
  reboot();
}

