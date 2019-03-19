# google-conn-test
The test measures connection time parameters to http://www.google.com/

## Usage
Linux/MacOS-compatible system is required. Porting would be required for Windows (e.g. sleep functionality). The provided makefile contains goals to build both the library and the application that uses it.
Possible options:
- H "Header-name: Header-value": custom HTTP requests (can be specified multiple times)
- n N: number of requests: if unspecified or set to 0 then adjusts it to GCONN_NUM_REQ (=10)
- i I: interval between requests: if unspecified or set to 0 then no delay is introduced

### Example
```
bin/gconn_app -H "User-Agent: gconn" -H "X-MyHeader: 123" -n 20 -i 1000
```
Note: script/test.sh contains various test combinations for the above

## Directory structure
- src/: library and application source code
- scripts/: test scripts
- tools/: checkpatch for code styling
- bin/: application executable (generated)
- lib/: library archive (generated)
- obj/: lbrary objects (generated)

## Output
Prints to stdout a string in the following format:
```
SKTEST;<IP address of HTTP server>;<HTTP response code>;<median of CURLINFO_NAMELOOKUP_TIME>;<median of CURLINFO_CONNECT_TIME>;<median of CURLINFO_STARTTRANSFER_TIME>;<median of CURLINFO_TOTAL_TIME>
```

### Example
```
SKTEST;216.239.38.120;200;0.062880;0.091319;0.164425;0.371414
```
Note: time values use double type representation with .6 digit precision

## gconn library API
|Function|Description|
|--------|------------|
|void gconn_init(void);|Initialize or reset internal structures|
|bool gconn_add_http_header(char *httpHeader);|Add custom HTTP header|
|bool gconn_set_num_req(int numReq);|Specify number of HTTP requests to make|
|bool gconn_set_interval_req(int reqInterval);|Specify interval between HTTP requests|
|resourceTiming_t *gconn_resource_timing_http_get();|Send HTTP requests and collect resource timing stats|

## Features
- creates a library object file as well as an archive file for static linking (shared object file creation not supported)
- uses libcurl v7.54.0 from Xcode, MacOS
- verified on Linux image (via Docker)
- supported parameters: custom HTTP headers, number of requests, interval between requests
- styled up using Linux checkpatch (non-strict)

## Known limitations
- does not check cross-traffic or current CPU load

## Things to improve
- use HEAD instead of GET (use CURLOPT_NOBODY) given response body is discarded anyway
- at the moment does not use shell specific env vars e.g. LD_LIBRARY_PATH
- logging not implemented
- single-threaded (not thread-safe)
- http headers format parsing is relied upon CURL
