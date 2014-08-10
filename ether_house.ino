#include <EtherCard.h>
#include <IPAddress.h>
#define REQUEST_RATE 50000 // milliseconds

static byte my_id = 1;
static byte my_mac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

// Easier to deal with up here than passing pointers
uint8_t target_mac[6] = { 0,0,0,0,0,0 };

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

  get_initial_config();
  
  ether.snifferListenForMac(&PrintPacket, target_mac);
  Serial.print("Enabling listener for MAC: ");
  printMac(target_mac);
    
}

void loop () {

  // Normal loop of getting packets if they are available
  ether.customPacketLoop(ether.packetReceive());

  
}



