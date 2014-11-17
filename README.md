## Etherhouse Client

Etherhouse a project of mine involving eight Christmas gifts.
Each gift involved a display of some model houses made from folded
paper, each representing the home of a friend or family member.

The houses light up, depending on whether that family member is
home or not. Their presence is detected based on if their smartphone
is on the same network the etherhouse is on.

### Hardware

Etherhouse involves a few hardware components:

* Arduino + ENC28J60 module
* Self-flickering LEDs
* Paper model houses
* Picture frame base

See the schematic (TODO) for electrical stuff.

### Software

The software includes two main components:

* Arduino firwmare (client)
** Listens for packets from the targets cell phone
** Updates the server when the target comes home or leaves
** Periodically polls the server for state updates
* [ether\_housed](https://github.com/solarkennedy/ether_housed) running on the internet (server)
** In charge of house => mac\_address mapping
** Store global state of who is home or not
** Does API key auth

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

