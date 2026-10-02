# C++ SNMP Network Management System Discovery

C++17 SNMPv2c discovery application using Net-SNMP.

## Requirements

- C++17 compiler
- CMake >= 3.16
- Net-SNMP development libraries

Debian/Ubuntu:

    sudo apt update
    sudo apt install build-essential cmake libsnmp-dev snmp

## Build

    mkdir build
    cd build
    cmake ..
    cmake --build . -j

## Run

    ./nms-discovery --subnet 192.168.1.0/24 --community public

JSON:

    ./nms-discovery --subnet 192.168.1.0/24 --community public \
        --output devices.json --format json

CSV:

    ./nms-discovery --subnet 192.168.1.0/24 --community public \
        --output devices.csv --format csv

Use only on networks you own or are authorized to administer.

This version uses SNMPv2c. For production, SNMPv3 is preferable because
SNMPv2c community strings are not encrypted.
