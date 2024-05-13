## HTTP Server

The process of creating a simple, single-threaded server can be divided into 2 steps.

### Socket File Descriptor
- Create the `socket fd` with `socket(...)`
- Bind said `socket fd` to some IP address (i.e your localhost)
    - Optionally, can invoke `setsockopt` to ensure the same `socket fd` can reuse the same IP address
- Start `listen`


### Interaction with Client
- Firstly, `accept` a connection request from the queue of pending connections. The return value will be the `client fd` for said connection.
- Invoke `read` to parse and store data from `client fd` in some buffer
- Similarly, call `write` to send data back to `client fd`

## Future

Plans to include multithreading, more complex parsing of HTTP requests, epoll sys calls