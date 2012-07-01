Hole Punch: UDP NAT Punching

(c) Chris Kennelly (chris@ckennelly.com)

Overview
========

This repository contains a simple example of UDP hole punching in C.  It
compiles to two binaries, a server that relays client information and a simple
client that ping-pongs data to and from another instance of itself.

This protocol may fail when two clients are behind the same NAT and encounter
a router that will not send packets to its own external IP address.

This example is licensed under the Boost Software License 1.0.  For more
information see COPYING.
