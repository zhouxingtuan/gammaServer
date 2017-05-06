
killall gamma

rm -rf node*

sleep 1

nohup ./gamma config1.ini -print >> node1.log 2>&1 &

sleep 1

nohup ./gamma config2.ini -print >> node2.log 2>&1 &
