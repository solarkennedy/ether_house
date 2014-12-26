# Etherhouse Client

![ether_houses](https://raw.githubusercontent.com/solarkennedy/ether_house/master/house_models/Frame/final_display.jpg)

Etherhouse a project of mine involving eight Christmas gifts.
Each gift involved a display of some model houses made from folded
paper, each representing the home of a friend or family member.

The houses light up, depending on whether that family member is
home or not. Their presence is detected based on if their smartphone
is on the same network the etherhouse is on.

## Hardware

Etherhouse involves a few hardware components:

* Arduino + ENC28J60 module
* Self-flickering LEDs
* Paper model houses
* Picture frame base

See the schematic (TODO) for electrical stuff.

### Arduino + Ethernet

The Arduino is a standard [Arduino Nano](http://arduino.cc/en/Main/arduinoBoardNano).

The Arduino connects to a ENC28J60 module via SPI using the [ethercard](https://github.com/jcw/ethercard)
library. I use modules with a built-in 3-5v converter so I can plug the two in directly.

### Building Models

One main component of the etherhouse project is the construction of the paper
models. As much as possible I tried to mimic the actual house owned by the
family member by using Bing/Google Maps arial photographs.

You can buy the
[same tea-light 3d model](http://www.silhouettedesignstore.com/?page=view-shape&id=69143)
 I used as a starting point if you want. 

I used the standard Silhouette software with a 
[Silhouette Portrait](http://www.silhouetteamerica.com/shop/machines/portrait)
to design, and cut the models. In reality, any cutting or CNC machine can do
this, depending on how handy you are.

See [house_models/README.md](https://github.com/solarkennedy/ether_house/tree/master/house_models/README.md)
for more instructions and pictures.

### Leds

To minimize components, I used a self-flickering yellow LED, sourced from Ebay.

Two LEDs are wired in serial for each house.

## Software

This repo contains the actual code that runs on the Arduino. 
This uses one external library and is in the native Arduino C++. This library
is slightly modified in order to sniff packets from a selected MAC address.

The Arduino runs a limited TCP/IP stack and interacts with the http api.

The code is writen with basic error detection and reboot capabilities, to ensure
the client continues to run without interruption or interaction. No one should
need to "turn it off and on again."

In summary:

* Arduino firwmare (client)
  * Queries the server for configruation (target mac address)
  * Listens for packets from the targets cell phone
  * Updates the server when the target comes home or leaves
  * Periodically polls the server for state updates
  * Saves state in EEPROM
* [ether\_housed](https://github.com/solarkennedy/ether_housed) running on the internet (server)
  * In charge of house => mac\_address mapping
  * Store global state of who is home or not
  * Does API key auth
  * Stores state of when each house last checked in

### Howto

* Make the hardware
* Pick secrets (API keys + MAC addresses)
* Program the arduino with a particular house\_id and api key
* Run the server with syncronized secrets

#### Syslog

The etherhouse client logs to broadcast syslog for debug purposes.

To listen for the messages, configure your syslog server. If you are using
rsyslog:

```
# cat 20-etherhouse.conf 
$RepeatedMsgReduction off
#news.* /var/log/etherhouse.log
:source, startswith, "etherhouse" /var/log/etherhouse.log

$ touch /varlog/etherhouse.log
$ chown syslog /varlog/etherhouse.log
$ restart rsyslog
$ tail -f /varlog/etherhouse.log
```

