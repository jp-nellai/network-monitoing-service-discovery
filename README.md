# NMS Discovery v2

C++17/Net-SNMP/SQLite prototype featuring SNMPv3 USM, GETBULK interface walking, vendor/device classification, SQLite persistence, REST JSON endpoint, embedded dashboard, concurrent discovery, and IPv4 CIDR expansion.

## Dependencies (Debian/Ubuntu)
`sudo apt install build-essential cmake libsnmp-dev libsqlite3-dev snmp`

## Build
`mkdir build && cd build && cmake .. && cmake --build . -j`

## Run
`./nms-discovery --subnet 192.168.1.0/24 --security-name nmsuser --auth-pass 'AUTH_PASSWORD' --priv-pass 'PRIV_PASSWORD' --security-level 3`

Dashboard: http://127.0.0.1:8080/  
API: http://127.0.0.1:8080/api/devices

Security levels: 1=noAuthNoPriv, 2=authNoPriv, 3=authPriv.

Use only on networks you are authorized to administer. For production add TLS/authentication to the API, secret storage, persistent SNMPv3 engine state, complete IP-MIB decoding, vendor MIBs, migrations, retention, metrics/alerting and a production frontend.
