#!/bin/sh

# build docker image
docker build -t gconn_app_memory_test:0.1 .

# run docker container for the image above
docker run -ti -v /Users/igor/Projects/google-conn-test:/test gconn_app_memory_test:0.1 bash -c "cd /test ; make && valgrind --leak-check=full bin/gconn_app"
