#define DBG_SNIFF 0

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

static uint8_t check_mac_message_is_from(const uint8_t *src_mac, const uint8_t *sniffed_mac) {
    return memcmp(src_mac, sniffed_mac, 6) == 0;
}

void EtherCard::snifferListenForMac(SnifferCallback callback, const uint8_t *srcmacaddr) {
    if(numSniffers < SNIFFER_MAXLISTENERS)
    {
        sniffers[numSniffers].callback = callback;
        memcpy(sniffers[numSniffers].srcmacaddr, srcmacaddr, sizeof sniffers[numSniffers].srcmacaddr);
        numSniffers++;
    }
}

bool EtherCard::snifferListening() {
    return numSniffers > 0;
}

#if DBG_SNIFF
extern void printMac(const uint8_t *macaddr);
#endif

void EtherCard::snifferProcessPacket(uint16_t plen) {
    const char *pkt_type;
    uint8_t *src_mac;
    uint8_t *src_ip;

#if DBG_SNIFF
Serial.print(F("sniffer called, plen="));
Serial.print(plen);
Serial.println();
#endif

    // Packet too short to have a full Ethernet header? -> ignore
    if (plen < ETH_HEADER_LEN)
    {
#if DBG_SNIFF
        Serial.print(F("plen "));
        Serial.print(plen);
        Serial.print(F("too short for Ethernet"));
        Serial.println();
#endif
        return;
    }

    src_mac = &gPB[ETH_SRC_MAC];

    // Find IP and source MAC based on packet layout
    // ARP packets
    if (gPB[ETH_TYPE_H_P] == ETHTYPE_ARP_H_V && gPB[ETH_TYPE_L_P] == ETHTYPE_ARP_L_V)
    {
        // Packet too short to have the fields we use? -> ignore
        if (plen < ETH_ARP_DST_MAC_P)
        {
#if DBG_SNIFF
            Serial.print(F("plen "));
            Serial.print(plen);
            Serial.print(F("too short for ARP"));
            Serial.println();
#endif
            return;
        }
        // We don't check HW type field, since we assume the HW is Ethernet
        // Make sure this is an IPv4 ARP packet, else ignore
        if ((gPB[ETH_ARP_PROTO_TYPE_H_P] != ETH_ARP_PROTO_IPV4_H_V) || (gPB[ETH_ARP_PROTO_TYPE_L_P] != ETH_ARP_PROTO_IPV4_L_V))
        {
#if DBG_SNIFF
            Serial.print(F("ARP proto "));
            Serial.print(gPB[ETH_ARP_PROTO_TYPE_H_P]);
            Serial.print(F(" "));
            Serial.print(gPB[ETH_ARP_PROTO_TYPE_L_P]);
            Serial.print(F(" not handled"));
            Serial.println();
#endif
            return;
        }
        // We don't check the HW or protocol address length field, since we know they must be 6, 4 for Ethernet, IPv4
        pkt_type = "ARP";
        src_ip = &gPB[ETH_ARP_SRC_IP_P];
    }
    // IP packets
    else if (gPB[ETH_TYPE_H_P] == ETHTYPE_IP_H_V && gPB[ETH_TYPE_L_P] == ETHTYPE_IP_L_V)
    {
        // Packet too short to have the fields we use? -> ignore
        if (plen < IP_DST_P)
        {
#if DBG_SNIFF
            Serial.print(F("plen "));
            Serial.print(plen);
            Serial.print(F("too short for IP"));
            Serial.println();
#endif
            return;
        }

        pkt_type = "IP";
        src_ip = &gPB[IP_SRC_P];
    }
    else
    {
#if DBG_SNIFF
        Serial.print(F("ETH_TYPE "));
        Serial.print(gPB[ETH_TYPE_H_P]);
        Serial.print(F(" "));
        Serial.print(gPB[ETH_TYPE_L_P]);
        Serial.print(F(" not yet handled"));
        Serial.println();
#endif
        return;
    }

#if DBG_SNIFF
    Serial.print(pkt_type);
    Serial.print(F(" packet from "));
    ether.printIp(src_ip);
    Serial.print(F(" "));
    printMac(src_mac);
    Serial.println();
#endif

    for(int i = 0; i < numSniffers; i++)
    {
        if(check_mac_message_is_from(src_mac, sniffers[i].srcmacaddr))
            sniffers[i].callback(src_mac, src_ip);
    }
}
