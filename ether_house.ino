
#include <JsonParser.h>
using namespace ArduinoJson::Parser;

#include <EtherCard.h>
#include <IPAddress.h>

#define REQUEST_RATE 50000 // milliseconds

const byte num_houses = 7;
const byte my_id = 0;
const byte my_mac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
uint8_t target_mac[6] = { -1,-1,-1,-1,-1,-1 };
byte state = -1;

const char api_server[] PROGMEM = "archive";

byte Ethernet::buffer[700];
static long timer;

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
  Serial.print(len);
  Serial.println(" words in length");
}

void setup () {
  Serial.begin(115200);
  Serial.println("\nether_house starting");
  
  if (ether.begin(sizeof Ethernet::buffer, my_mac) == 0) 
    Serial.println( "Failed to access Ethernet controller");

  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
  
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
  
  if (!ether.dnsLookup(api_server)) {
    Serial.println("DNS failed");
    // TODO: Sleep then reset
  }
  ether.printIp("Server: ", ether.hisip);
 
  set_target_mac();
  set_initial_state(); 
  
  Serial.println("Finished initial configuration");
        
  //ping_sweep();
  Serial.println("Now entering main loop");
}

void loop () {

  // Normal loop of getting packets if they are available
  ether.customPacketLoop(ether.packetReceive());

  
}



