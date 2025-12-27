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

## References

* [Beej's guide to network programming](https://beej.us/guide/bgnet/)

