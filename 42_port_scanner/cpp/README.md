# Port scanner

## Description

1. Probe specific ports on a network host, using both UDP and TCP
2. Input: protocol selection, host, port(s)
3. Output: port state (OPEN|CLOSED) for each input port

## Probing logic

### TCP

* Concept: try to initiate a TCP connection (3-way handshake)
* Outcome:
  * Connection succeeds: port is open
  * Connection refused/reset: port is closed
  * Timeout: possibly filtered or host unreachable (optional to handle)

### UDP

* Concept: send an empty UDP packet to the port
* Outcome:
  * ICMP "Port Unreachable" response: post is closed
  * No response: port is open or filtered
  * optional: if the UDP service replies (e.g. DNS), it's obviously open

## Considerations

* Handle timeout graciously
* Requests in parallel for a large port input count
* Time how long a scan takes
* Show summary stats (ports scanned, open/closed count, duration)

## TODO

* Parse input hosts as CIDR (e.g. `10.0.0.0/24`) to generate set of hosts
* Split hosts into equal size sets and execute each set in parallel
* Improve protocol handling, only doing TCP at the moment

## References

* [Beej's guide to network programming](https://beej.us/guide/bgnet/)
* [socket() - Linux manpage](https://www.man7.org/linux/man-pages/man2/socket.2.html)
* [getaddrinfo() - Linux manpage](https://www.man7.org/linux/man-pages/man3/getaddrinfo.3.html)
* [select() - Linux manpage](https://www.man7.org/linux/man-pages/man2/select.2.html)
* [netdb.h - Linux manpage](https://www.man7.org/linux/man-pages/man0/netdb.h.0p.html)
