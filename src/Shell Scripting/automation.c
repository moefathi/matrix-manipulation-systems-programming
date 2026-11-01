#!/bin/bash

echo "Compiling..."

gcc server.c -o server.o -pthread
gcc client.c -o client.o

rm -rf clientOutputs
mkdir clientOutputs

echo -n "" > serverLog.txt

chmod 600 serverLog.txt

echo "Starting Server..."

./server.o &

serverPID=$!

sleep 1

echo "Starting 50 Clients..."

clientPids=""

for (( i = 1 ; i <= 50; i++ ))
do

 ./client.o > clientOutputs/client_$i.txt &

 clientPids="$clientPids $!"

done

wait $clientPids

sleep 1

echo "Connected Clients:"

grep -cE "^Client [0-9]+ Connected$" serverLog.txt

echo "Disconnected Clients:"

grep -cE "^Client [0-9]+ Disconnected$" serverLog.txt

echo "Client Records:"

grep -nE "^Client [0-9]+ (Connected|Disconnected)$" serverLog.txt

echo "Calculation Results:"

grep -nE "(Multiplication Result|Transpose Of Matrix [AB]|Average Of Matrix [AB])" serverLog.txt

echo "Client Choices:"

grep -rnE "(Transpose Choice: [AB]|Average Choice: [AB])" clientOutputs

kill $serverPID

echo "Automation Finished"
