#!/bin/sh

../bin/gconn_app -H "User-Agent: gconn" -H "X-MyHeader: 123" -n -5 -i 1000
../bin/gconn_app -n -45 -i -100
