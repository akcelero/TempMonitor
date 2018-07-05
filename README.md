# TempMonitor

Code for arduino with shield W5100 and up to 25 DS18B20 sensors.
Device send XML with measurements to server, but also has telnet interface for managing following settings:
- DHCP / static IP
- static IP / DNS / subnet / gateway
- period (between sending data)
- url (of server)
- token (for security)
- aliases (addresses <-> human readable names)
