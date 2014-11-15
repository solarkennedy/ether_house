== Etherhouse Client

=== Syslog

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

