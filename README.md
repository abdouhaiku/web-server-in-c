# web-server-in-c

A small HTTP/1.1 server written from scratch in C, built as a learning project for sockets, manual memory management, concurrency, and the HTTP protocol.

## What it does

- Accepts TCP connections and parses raw HTTP requests, including query string parameters (`?key=value&...`).
- Routes requests by method + path to handler functions via a small router (`router_add`/`router_match`); unmatched requests fall back to serving a static file from `resource/assets/`.
- Detects the MIME type from the file extension and serves the file's raw bytes with a `200 OK` response, or a `404 Not Found` if the file doesn't exist.
- Ships a few example routes (`application_routes.c`) demonstrating the handler pattern: a JSON file response, a query-param-driven response, and a POST body echo.
- Responses are assembled generically via a `response_t` struct, `build_headers`, and `send_response`, so every response path (success or error) goes through the same serialization code.
- Handles connections with a **bounded queue + thread pool** instead of one thread per connection: the accept loop pushes accepted connections onto a fixed-capacity queue, and a fixed set of worker threads pull from it and process them. If the queue is full, the accept loop blocks until a worker frees a slot. The connections are queued following the ring buffer algorithm

## Project layout

```text
web-server-in-c/
├── CMakeLists.txt
├── config.ini
├── resource/            # static assets served from disk
├── include/             # all headers
│   ├── utilities.h
│   ├── server_config.h
│   ├── router.h
│   ├── application_routes.h
│   └── queue.h
└── src/                 # all source files
    ├── webserver.c       # main(): config/socket setup, worker pool spawn, accept loop
    ├── utilities.c        # request parsing, response serialization, static file handling
    ├── server_config.c    # config.ini parsing, socket creation
    ├── router.c           # method+path -> handler dispatch table
    ├── application_routes.c  # example route handlers
    └── queue.c            # bounded connection queue (producer/consumer)
```

## Configuration

Server settings are read from `config.ini` (must be in the working directory the binary is run from), under a single `[server_config]` section:

```ini
[server_config]
port=8081
backlog=10
bind_address=127.0.0.1
maximum_of_connections=16
number_worker_threads=4
```

| Key | Meaning |
|---|---|
| `port` | TCP port the server listens on |
| `backlog` | OS-level `listen()` backlog — how many fully-handshaked connections the kernel will hold before your app calls `accept()` |
| `bind_address` | IP address to bind the listening socket to |
| `maximum_of_connections` | Capacity of the app-level connection queue between the accept loop and the worker pool |
| `number_worker_threads` | Number of long-lived worker threads processing connections from the queue |

## Building & running

```sh
cmake -B build
cmake --build build
./build/web_server
```

Run it from the project root (or copy `config.ini` and `resource/` alongside the binary),  both are read via relative paths at startup. The server listens on `http://<bind_address>:<port>/` as configured in `config.ini` (`http://127.0.0.1:8081/` by default).

## Possible improvements

Not implemented yet, in no particular order:

- **HTTPS** : TLS termination (e.g. via OpenSSL) so the server can accept `https://` connections instead of plaintext-only HTTP.
- **Request timeout handling** : bound how long a worker will wait on a slow/stalled client (e.g. during `read()`), so one misbehaving connection can't tie up a worker thread indefinitely.
- **A small middleware system** : a way to run shared logic (logging, auth, headers) before/after route handlers without every handler reimplementing it.
- **Rate limiting** : cap how many requests a client can make in a given time window, to protect the server from being overwhelmed by a single source.
