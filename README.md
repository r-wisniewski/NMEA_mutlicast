# Usage

```
Usage: ./NMEA_Multicast [Local Interface IP Address]

Parameter(s):
	- Local Interface IP Address: The IP address of the interface outputting the messages
```
# Purpose

This program will mutlicast a predefined NMEA string to a predefined multicast group. Future work will support adding a config file to determine which messages are output and associated frequencies. Another addition will be a “worker” thread to take in data and update varibles such as latitude, longitude, etc…. This will require mutexes.

Compile: cc NMEA_multicast.c -o NMEA_multicast -lpthread 

# Improvements

Thoughts to improve the NMEA_Multicast program are:

1. Pass arguments in via a config file. This would allow much more flexibility and simplify the process of setting the system up to output many NMEA messages.

# Contact

Robin Wisniewski – [LinkedIn](https://www.linkedin.com/in/robin-wisniewski/) –  [wisniewski.ro@gmail.com](mailto:wisniewski.ro@gmail.com)
