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
    uint16_t port;
    bool listening;
} snifferListener;

snifferListener sniffers[SNIFFER_MAXLISTENERS];
byte numSniffers = 0;

void EtherCard::snifferListenOnPort(SnifferCallback callback, uint16_t port) {
    if(numSniffers < SNIFFER_MAXLISTENERS)
    {
        sniffers[numSniffers] = (snifferListener){callback, port, true};
        numSniffers++;
    }
}

void EtherCard::snifferPauseListenOnPort(uint16_t port) {
    for(int i = 0; i < numSniffers; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (sniffers[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) sniffers[i].port)) {
            sniffers[i].listening = false;
        }
    }
}

void EtherCard::snifferResumeListenOnPort(uint16_t port) {
    for(int i = 0; i < numSniffers; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (sniffers[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) sniffers[i].port)) {
            sniffers[i].listening = true;
        }
    }
}

bool EtherCard::snifferListening() {
    return numSniffers > 0;
}

bool EtherCard::snifferHasProcessedPacket(uint16_t plen) {
    bool packetProcessed = false;
    for(int i = 0; i < numSniffers; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (sniffers[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) sniffers[i].port) && sniffers[i].listening)
        {
            uint16_t datalen = (uint16_t) (gPB[UDP_LEN_H_P] << 8)  + gPB[UDP_LEN_L_P] - UDP_HEADER_LEN;
            sniffers[i].callback(
                sniffers[i].port,
                gPB + IP_SRC_P,
                (const char *) (gPB + UDP_DATA_P),
                datalen);
            packetProcessed = true;
        }
    }
    return packetProcessed;
}
