#!/bin/sh


#IP=127.0.0.1
IP=192.168.0.125
#IP=192.168.0.218
# my local system
# print type sting
snmpset -h 2>&1| tail -4

echo "set tunner 1 param "
snmpset -v2c -c private $IP 1.3.6.1.3.5188.19.0 s 0

sleep 2

echo "start psi analyse"
# fp
#snmpset -v2c -c private $IP 1.3.6.1.3.5188.15.0 s 22
# snmp 
snmpset -v2c -c private $IP 1.3.6.1.3.5188.15.0 s 11
exit 0



echo "get tunner 1 param"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.19.0 s 0
exit 0

echo "get sdt"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.2.5.1.0

exit 0
echo "start psi analyse"
# fp
#snmpset -v2c -c private $IP 1.3.6.1.3.5188.15.0 s 22
# snmp 
snmpset -v2c -c private $IP 1.3.6.1.3.5188.15.0 s 11
exit 0

echo "get psi analyse status"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.16.0 s 0
exit 0






echo "get psi analyse status"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.16.0 s 0

exit 0


echo "test data size"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.200.0
exit 0


echo "test down pid map"
snmpset -v2c -c private $IP 1.3.6.1.3.5188.15.0 s 33
exit 0



# apply section -> ts psi
snmpset -v2c -c private $IP 1.3.6.1.3.5188.17.0 s 1111111
exit 0







echo "get pmt"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.2.1.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.2.2.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.2.3.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.2.23.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.2.24.0



# set net
snmpset -v2c -c private $IP 1.3.6.1.3.5188.13.0 s ff12341244ffff1255ffffff
exit 0



echo "get nit"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.4.0
exit 0


echo "test data size"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.200.0
exit 0

echo "get sdt"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.5.1.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.5.2.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.5.3.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.5.4.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.5.5.0
exit 0

snmpget -v2c -c public $IP 1.3.6.1.3.5188.15.0
snmpget -v2c -c public $IP 1.3.6.1.3.5188.15.0
echo "get pat"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.1.0
#snmpset -v2c -c private $IP 1.3.6.1.3.5188.1.1.0 s "11111111"

echo "start psi analyse"
snmpset -v2c -c private $IP 1.3.6.1.3.5188.15.0 s 1

echo "get pmt2"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.2.2.0

echo "psi status"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.16.0

echo "get PAT 0"
snmpget -v2c -c public $IP 1.3.6.1.3.5188.1.1.0
#snmpget -v2c -c public $IP 1.3.6.1.4.1.5188.1.4.8.1.32.0
#snmpget -v2c -c public $IP 1.3.6.1.4.1.5188.1.4.8.2.2.0

exit 0

snmpget -v2c -c pub localhost 1.3.6.1.4.1.5188.1.4.8.1.32.0
#snmpget -v2c -c pub localhost 1.3.6.1.4.1.5188.1.4.8.1.32.1
snmpset -v2c -c private localhost 1.3.6.1.4.1.5188.1.4.8.1.32.0 u 1
snmpget -v2c -c pub localhost 1.3.6.1.4.1.5188.1.4.8.1.32.0
snmpget -v2c -c pub localhost 1.3.6.1.4.1.5188.1.4.8.2.1.0

# start analyse, 0x55AA=21930
snmpset -v2c -c private localhost 1.3.6.1.4.1.5188.1.4.15.0 u 21930
# stop analyse
snmpset -v2c -c private localhost 1.3.6.1.4.1.5188.1.4.15.0 u 0

# bin
snmpget -v2c -c pub localhost 1.3.6.1.4.1.5188.1.4.8.2.2.0
