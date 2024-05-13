## HTTP Server

## Build
To start server:
```
cd build
cmake ..
cmake --build .
```

To mock client:
```
curl -i http://localhost:8080
```

## Notes (mainly for myself)

The process of creating a simple, single-threaded server can be divided into two steps.

### Socket File Descriptor
- Create the `socket fd` with `socket(...)`
- Bind said `socket fd` to some IP address (i.e your localhost)
    - Optionally, can invoke `setsockopt` to ensure the same `socket fd` can reuse the same IP address
- Start `listen`

  <img width="807" alt="Screenshot 2024-05-14 at 1 45 41 AM" src="https://github.com/btjm123/http-server/assets/19306879/e5009c15-51e4-4921-a10b-ea5b977ca043">

### Interaction with Client
- Firstly, `accept` a connection request from the queue of pending connections. The return value will be the `client fd` for said connection.
- Invoke `read` to parse and store data from `client fd` in some buffer
- Similarly, call `write` to send data back to `client fd`
