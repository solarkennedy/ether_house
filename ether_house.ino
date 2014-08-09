#include <EtherCard.h>
#define REQUEST_RATE 50000 // milliseconds


static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

static byte theirmac[] = { 0x60,0xbe,0xb5,0x8f,0x3d,0xa7 };

const char website[] PROGMEM = "archive";
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
  Serial.println("\n[getDHCPandDNS]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");

  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
  
  ether.printIp("My IP: ", ether.myip);
  // ether.printIp("Netmask: ", ether.mymask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
  ether.printIp("Server: ", ether.hisip);
  
  timer = - REQUEST_RATE; // start timing out right away
}

void loop () {
 /*   
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer + REQUEST_RATE) {
   timer = millis();
   Serial.println("\n>>> REQ");
   ether.browseUrl(PSTR("/foo/"), "bar", website, my_result_cb);
 }
 */
 

  word pos = ether.customPacketLoop(ether.packetReceive());
  if (pos) {
    char* data = (char *) Ethernet::buffer + pos;
    Serial.println();
    Serial.println("Got data?");
    Serial.println(data);
  }
}


