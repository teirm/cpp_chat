# Design Document

## Overview 

Design and protocol documentation for chat client and server.

## Server

Threaded server using IO multiplexing to handle mutliple 
client connections. 

Accepted connections will be added to multiplexing
list and also stored in sever table. All sockets
should be non-blocking.

If socket is ready for reading, read message and 
write it to all applicable recipients. All recipients may
be sender, a specific other client, or all other clients.

Each client connected to server will maintain a last
active time. In case capacity is met on server, the 
least recently active client will be disconnected.

### Server Error Handling

Cases requiring handling by the server.

1. Client disconnects with shutdown - handle 0 read
1. Client crashes - handle ECONNRESET

In both cases, client should be removed from multiplexing 
sets and cache. 

## Client

TODO

## Protocol

Define constant:

* ```MSG_DATA_MAX_SIZE = 400`` 

This defines the the maximum message size from the client
to the server and from the server to the client.

Messages will be treated as std::strings except at the
lowest level where they will be read from the socket
into an ```char[]```.

Message data will be preceded by a header containing the 
following:

* ```u_int_16 length```
* ```u_int_64 timestamp```
* ```char *target```

Overall a message will be defined as the struct:

```c
struct {
    uint32_t      msg_len;
    uint64_t      timestamp; /* second since epoch */
    char*         target;
    char*         msg;
}
```

The client and server will be expected to abide by the 
following interpertation of the above fields and
values:

* Reading a Message
  * Read message header first by reading 
  * Use header information to verify ```length``` is less
    than ```MSG_DATA_MAX_SIZE```
    * If ```length``` is greater than ```MSG_DATA_MAX_SIZE```
      treat it as an error 
    * If ```length``` is less than ```MSG_DATA_MAX_SIZE```
      read ```length``` bytes
  * If server, use non-nullptr ```target``` to direct message
    * nullptr ```target``` field will be treated as a broadcast

* Writing a Message
  * Verify message is less than ```MSG_DATA_MAX_SIZE``` bytes
    * If message is greater than ```MSG_DATA_MAX_SIZE``` bytes
      return an error
  * Populate header with 
  * Write message
