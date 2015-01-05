// Simple packet sniffer
//
// Author: Brian Lee
//
// Copyright: GPL V2
// See http://www.gnu.org/licenses/gpl.html

#include "EtherCard.h"
#include "net.h"

#define gPB ether.buffer

#define SNIFFER_MAXLISTENERS 8    //the maximum number of port listeners.

typedef struct {
    SnifferCallback callback;
    uint8_t srcmacaddr[6];
} snifferListener;

snifferListener sniffers[SNIFFER_MAXLISTENERS];
byte numSniffers = 0;


static uint8_t check_mac_message_is_from(const uint8_t *mac) {
    return memcmp(gPB + ETH_SRC_MAC, mac, 6) == 0;
}

void EtherCard::snifferListenForMac(SnifferCallback callback, const uint8_t *srcmacaddr) {
    if(numSniffers < SNIFFER_MAXLISTENERS)
    {
        sniffers[numSniffers] = (snifferListener){callback, *srcmacaddr };
        // I don't really know what I'm doing. Pointers?
        for ( int i=1; i<6; i++) {
            sniffers[numSniffers].srcmacaddr[i] = srcmacaddr[i];
        }
        numSniffers++;
    }
}

bool EtherCard::snifferListening() {
    return numSniffers > 0;
}

bool EtherCard::snifferProcessPacket(uint16_t plen) {
    for(int i = 0; i < numSniffers; i++)
    {
        if(check_mac_message_is_from(sniffers[i].srcmacaddr))
            sniffers[i].callback(gPB + ETH_SRC_MAC, gPB + IP_SRC_P);
    }
    return true;
}
