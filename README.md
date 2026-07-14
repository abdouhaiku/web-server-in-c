# web-server-in-c

A small HTTP/1.1 server written from scratch in C, built as a learning project for sockets, manual memory management, and the HTTP protocol.

## What it does

- Accepts TCP connections and parses raw HTTP requests.
- Resolves the request path to a file under `resource/assets/`
- Detects the MIME type from the file extension and serves the file's raw bytes with a `200 OK` response, or a `404 Not Found` if the file doesn't exist.
- Responses are assembled generically via a `response_t` struct, `build_headers`, and `send_response`, so every response path (success or error) goes through the same serialization code.

## Building & running

```sh
cmake -B build
cmake --build build
./build/web_server
```

The server listens on `http://127.0.0.1:8081/`.

## Known limitations

- **Single-threaded and blocking** — one connection is handled fully (accept → read → respond → close) before the next is accepted. A slow or idle (keep-alive) client connection stalls every other pending request — this is why a page reload can sometimes show as "pending" in the browser.
- No `Connection: close` header is sent, so HTTP/1.1 clients may attempt to keep connections alive.
- `fread`'s return value isn't checked for short reads or I/O errors.

## Next steps

- Add concurrency (thread- or process-per-connection, or a `select`/`poll`/`epoll`-based event loop) so one stalled client can't block others.
