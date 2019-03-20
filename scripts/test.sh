#!/bin/sh

../bin/gconn_app -H "User-Agent: gconn" -H "X-MyHeader: 123" -n -5 -i 1000
../bin/gconn_app -H "X-MyHeader: 456" -n 25
../bin/gconn_app -n -45 -i -100
../bin/gconn_app -n -45 -i -100 2>/dev/null
../bin/gconn_app
