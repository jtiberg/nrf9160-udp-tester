# nrf9160-udp-tester
Simple application for testing IP connectivity on Nordic Semiconductors nRF9160.

## Building the application (on Linux)
Make sure you have Nordic Semiconductors Connect SDK installed (https://github.com/NordicPlayground/fw-nrfconnect-nrf)

Set environment variable NCS_BASE to Connect SDK base path (or substitute it in CMakeLists.txt)

Set host of UDP server (CONFIG_UDP_SERVER_HOST) in prj.conf. You can also change port and interval if you'd like.

### Example build (via CLI):
mkdir build-dk && cd build-dk && cmake -GNinja -DBOARD=$board .. && ninja


## Example UDP server
Use the python3 application server/udp_server.py (prints time of recv, origin and message content) OR just netcat (nc -uvkl 42501)


## Why does this project exist?
I noticed weird issues with Telia LTE-M in Gothenburg/Sweden 2019-08.

This program revealed IP connectivity is present 2-3 seconds, lost for 10 seconds, present for 2-3 seconds and so on. This is visible by looking at the received UDP messages on the server (application does not know about this).

When setting the interval to 500ms, the issues disappear. Longer interval, like 600 or 1000ms (default in the project), the issues are present.
