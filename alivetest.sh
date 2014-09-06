while [ 1 ]; do

  ping -W 5 -c 4 192.168.1.76
  if [ $? -ne 0 ]; then 
    notify-send "PING FAILED"
    exit 1
  fi

done
