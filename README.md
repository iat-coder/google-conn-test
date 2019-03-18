# google-conn-test
The test measures connection time parameters to http://www.google.com/

Options:
========
-H "Header-name: Header-value": custom HTTP requests (can be specified multiple times)
-n N: number of requests: if unspecified or set to 0 then adjusts it to GCONN_NUM_REQ (=10)
-i I: interval between requests: if unspecified or set to 0 then no delay is introduced

Features:
=========
- creates a library object file as well as an archive file for static linking (shared object file creation not supported)
- uses libcurl v7.54.0 from Xcode, MacOS
- also checked on Linux image (via Docker)
- supported parameters: custom HTTP headers (up to 50, <256 long each), number of requests, interval between requests
- styled using Linux checkpatch
- Makefile contains goals to build both the library and the application that uses it
- time values use double type, .6 digit precision

Known limitations:
==================
- does not check cross-traffic or current CPU load

Things to improve:
==================
- use HEAD instead of GET (use CURLOPT_NOBODY) given response body is discarded anyway
- at the moment does not use shell specific env vars e.g. LD_LIBRARY_PATH
- logging not implemented
- single-threaded (not thread-safe)
- http headers format parsing - rely on CURL

?
- make scripts executable
- total time?
- malloc return values
- stderr
