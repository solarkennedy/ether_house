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
    bool listening;
} snifferListener;

snifferListener sniffers[SNIFFER_MAXLISTENERS];
byte numSniffers = 0;


static uint8_t check_mac_message_is_from(const uint8_t *mac) {
    return memcmp(gPB + ETH_SRC_MAC, mac, 6) == 0;
}

void EtherCard::snifferListenForMac(SnifferCallback callback, uint8_t srcmacaddr[6]) {
    if(numSniffers < SNIFFER_MAXLISTENERS)
    {
        sniffers[numSniffers] = (snifferListener){callback, srcmacaddr[6], true};
        numSniffers++;
    }
}
/*
void EtherCard::snifferPauseListenForMac(uint8_t srcmacaddr[6]) {
    for(int i = 0; i < numSniffers; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (sniffers[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) sniffers[i].port)) {
            sniffers[i].listening = false;
        }
    }
}

void EtherCard::snifferResumeListenForMac(uint8_t srcmacaddr[6]) {
    for(int i = 0; i < numSniffers; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (sniffers[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) sniffers[i].port)) {
            sniffers[i].listening = true;
        }
    }
}
*/

bool EtherCard::snifferListening() {
    return numSniffers > 0;
}

bool EtherCard::snifferProcessPacket(uint16_t plen) {
    for(int i = 0; i < numSniffers; i++)
    {
        if(sniffers[i].listening)
       // if(check_mac_message_is_from(sniffers[i].srcmacaddr) && sniffers[i].listening)
        {
            uint16_t datalen = (uint16_t) (gPB[UDP_LEN_H_P] << 8)  + gPB[UDP_LEN_L_P] - UDP_HEADER_LEN;
            sniffers[i].callback(
                gPB + ETH_SRC_MAC,
                gPB + IP_SRC_P,
                (const char *) (gPB + UDP_DATA_P),
                datalen);
        }
    }
    return true;
}
