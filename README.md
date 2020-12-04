# Usage

```
Usage: ./Controller
```

The controller program will fork() and execv() a number of NMEA_multicast processes to allow the output multiple NMEA messages at varying frequencies. For now the argument vector is hard coded and the structure of the fork() + execv()’s is as well.

To output a single NMEA message at a certain frequency use the NMEA_multicast script below:
```
Usage: ./NMEA_Multicast [Multicast_IPv4_Addr] [Port_Number] [Local_Multicast_Interface] [Frequency] [NMEA_Message]
   Parameters:
   - Multicast_IPv4_Addr: IPv4 mutlicast group address. Between 224.0.0.0 and 239.255.255.255
   - Port_Number: Port number of the multicast group
   - Local_Multicast_Interface: Local interface of server that outputs packets/messages to multicast group
   - Frequency: Output frequency of message
   - NMEA_Message: NMEA message to output
```
# Improvements

Thoughts to improve the controller program are:

1. Pass arguments in via a config file. This would allow much more flexibility and simplify the process of setting the system up to output many NMEA messages.

2. Fix the fork() + execv() structure. With the structure hardcoded, the code must be modified and compiled for each set of messages.

# Contact

Robin Wisniewski – [LinkedIn](https://www.linkedin.com/in/robin-wisniewski/) –  [wisniewski.ro@gmail.com](mailto:wisniewski.ro@gmail.com)
