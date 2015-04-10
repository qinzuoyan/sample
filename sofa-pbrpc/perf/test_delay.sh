#!/bin/sh

#############################################
HOST=$HOSTNAME
PORT=12345
SERVER_THREAD_NUM=4
SERVER_LOG=__test_delay.server.log
CLIENT_LOG=__test_delay.client.log
#############################################

SERVER=../../../build64_release/sample/sofa-pbrpc/perf/server
CLIENT=../../../build64_release/sample/sofa-pbrpc/perf/client

if ! [ -f $SERVER ] || ! [ -f $CLIENT ]
then
    echo "binary not generated, to make!"
    exit 1
fi

function at_exit()
{
    killall $SERVER &>/dev/null
    killall $CLIENT &>/dev/null
    exit 1
}

trap 'at_exit' INT QUIT
killall $SERVER &>/dev/null
killall $CLIENT &>/dev/null

echo "$SERVER $HOST $PORT $SERVER_THREAD_NUM &>$SERVER_LOG &"
$SERVER $HOST $PORT $SERVER_THREAD_NUM &>$SERVER_LOG &
sleep 1

echo
echo "============= test 1K data =============="
echo "$CLIENT $HOST $PORT 950 &>$CLIENT_LOG &"
$CLIENT $HOST $PORT 950 &>$CLIENT_LOG &
echo "sleep 1"
sleep 1
killall $CLIENT &>/dev/null
grep -o -a 'elapsed time in us: [0-9]*$' $CLIENT_LOG \
| awk 'BEGIN{sum=0;num=0}{if(NR > 10){sum+=$5;++num;}}END{ \
print "Succeed count: " num; \
print "Average elapsed time: " (sum/num) "us";}'

echo
echo "============= test 1M data =============="
echo "$CLIENT $HOST $PORT 1048576 &>$CLIENT_LOG &"
$CLIENT $HOST $PORT 1048576 &>$CLIENT_LOG &
echo "sleep 10"
sleep 10
killall $CLIENT &>/dev/null
grep -o -a 'elapsed time in us: [0-9]*$' $CLIENT_LOG \
| awk 'BEGIN{sum=0;num=0}{sum+=$5;++num;}END{ \
print "Succeed count: " num; \
print "Average elapsed time: " (sum/num) "us";}'

echo
echo "============= test 10M data =============="
echo "$CLIENT $HOST $PORT 10485760 &>$CLIENT_LOG &"
$CLIENT $HOST $PORT 10485760 &>$CLIENT_LOG &
echo "sleep 10"
sleep 10
killall $CLIENT &>/dev/null
grep -o -a 'elapsed time in us: [0-9]*$' $CLIENT_LOG \
| awk 'BEGIN{sum=0;num=0}{sum+=$5;++num;}END{ \
print "Succeed count: " num; \
print "Average elapsed time: " (sum/num) "us";}'

echo
at_exit

